/*
 *	main.c - Constraint City WIFI scanning & servo actuator application 
 *
 *	Copyright Gottfried Haider & Gordan Savicic 2008-2012.
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Lesser General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <nds.h>
#include <stdio.h>
#include <time.h>
#include <dswifi9.h>

#include "aplist.h"
#include "uart.h"
#include "brut.h"

#include "cclogo.h"

#define SERVO_PIN1 PD3
#define SERVO_PIN2 PD4
#define SERVO_PIN3 PD6

// works with ds brut firmware version 0x14
// tested with DS brut (master) rev. a & rev. b
int usleep(unsigned long usec)
{
    swiDelay(4190 * usec / 1000);
}

int main(void)
{
	aplist *head = NULL;
	aplist *cur;
	
	bool attached = true;
	// change this to false if you are only testing servos
	bool wifi_scan = true;
	// change this for emulator 
	bool emulate = false;

	float pain = 0.9f;
	time_t update = 0;
	uint8 num;
	uint8 servo_pos = 0;
	uint8 current_servo = 1;
	unsigned char SERVO_PINS[3] = { SERVO_PIN1 ,SERVO_PIN2 ,SERVO_PIN3 } ;

	uint16 val[3] = { 0 };
	uint8 i;

	touchPosition touch;
	
	videoSetMode(MODE_4_2D);
	vramSetBankA(VRAM_A_MAIN_BG);

	// set up our bitmap background
	bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	decompress(cclogoBitmap, BG_GFX,  LZ77Vram);

	// initialise lower screen for textoutput
	consoleDemoInit();

	if (emulate == false) {	

	  iprintf("Initializing WiFi.. ");
	  Wifi_InitDefault(false);
	  while (Wifi_CheckInit() != 1) {
	  }
	  Wifi_ScanMode();
	  iprintf("done\n");
	  
	  iprintf("Initializing DS brut.. ");
	  uart_init();
	  uart_set_spi_rate(1000);
	  iprintf("done\n\n\n");

	  iprintf("Using servo pins: %u %u %u\n\n", SERVO_PIN1, SERVO_PIN2, SERVO_PIN3);
	  iprintf("Default pain multiplier: %.2f\n\n", pain);
	  swiDelay(60000000);
	  while (1) {
		  
		  scanKeys();
		  touchRead(&touch);	
		  if (keysDown() & KEY_X) {
			  if (attached) {
				  servo_detach(SERVO_PIN1);
				  servo_detach(SERVO_PIN2);
				  servo_detach(SERVO_PIN3);
				  attached = false;
			  } else {
				  attached = true;
			  }
		  }
		  if (keysDown() & KEY_A) {
			  if (attached) {
				uint8 i = 0;
				for (i=0;i<3;i++) {
					servo_set(SERVO_PINS[i],0);
				}
			  }
			  //servo_set(SERVO_PIN1, 180-((rand() % 100)+(rand() % 50)+(rand() % 25)));
			  //servo_set(SERVO_PIN2, 180-((rand() % 100)+(rand() % 50)+(rand() % 25)));
			  //servo_set(SERVO_PIN3, 180-((rand() % 100)+(rand() % 50)+(rand() % 25)));
		  }
		  if (keysDown() & KEY_B) {
			  if (wifi_scan == true)	{
				  wifi_scan = false;
			  }
			  else {
				  wifi_scan = true;
			  }
		  }
		  if (keysDown() & KEY_DOWN) {
			  pain -= 0.1f;
			  if (pain < 0.0f) {
				  pain = 0.0f;
			  }
			  update = time(NULL);
		  }
		  if (keysDown() & KEY_UP) {
			  pain += 0.1f;
			  if (2.0f < pain) {
				  pain = 2.0f;
			  }
			  update = time(NULL);
		  }
		  if (keysDown() & KEY_L) {
			  current_servo += 1;
			  if (current_servo > 3) {
				  current_servo = 1;
			  }
		  }
		  consoleClear();
		  if (wifi_scan == true) {
			num = 0;
			cur = head;
			iprintf("\n");
			while (cur && num < 15) {
				// display
				if (!(cur->flags & 0x2)) {
					cur = cur->next;
					continue;
				}
				iprintf("%2u ", num);
				if (cur->ssid[0] == '\0') {
					iprintf("%02x%02x%02x%02x%02x%02x", cur->mac[0], cur->mac[1], cur->mac[2], cur->mac[3], cur->mac[4], cur->mac[5]);
				} else {
					iprintf("%s", cur->ssid);
				}
				iprintf(" @ %u", cur->rssi);
				if ((cur->flags & 0x06) == 0x06) {
					iprintf(" WPA");
				} else if (cur->flags & 0x02) {
					iprintf(" WEP");
				}
				
				// calculate servo commands
				if (attached && num < 3) {
						val[num] = (uint16)(cur->rssi*pain);
						if (180 < val[num]) {
							val[num] = 180;
						}
						iprintf(" %u", val[num]);
				}
				
				iprintf("\n");
				if (num == 2) {
					iprintf ("\n");
				}
				
				num++;
				cur = cur->next;
			}
			iprintf("\n");
			if (time(NULL) < update+3) {
				printf("\npain multiplier: %.2f\n", pain);
			}
			
			// set the servo to zero if we don't have enough wifi nodes
			for (i=num; i<3; i++) {
				val[i] = 0;
			}
			
			if (attached) {
				servo_set(SERVO_PIN1, (uint8)val[0]);
				servo_set(SERVO_PIN2, 180-(uint8)val[1]);
				servo_set(SERVO_PIN3, (uint8)val[2]);
			}
			updateApList(&head, 0x00, NULL, NULL);
			sortApList(&head, false);

		  }

		  if (KEY_TOUCH && attached && !wifi_scan) {
			  servo_pos = (touch.rawx / 3850.) * 180;	
			  iprintf ("servo pos x: %d;", servo_pos);
			  iprintf ("\n");
			  servo_set(SERVO_PINS[current_servo-1],servo_pos);
			  iprintf ("current servo: %d", current_servo);
			  iprintf ("\n\n");
		  }
		  
		  
		  swiWaitForVBlank();
	  }
 	}	
while (1) {
		  swiWaitForVBlank();
}
	return 0;
}
