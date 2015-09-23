
## Software Android


```
cordova create projectname
cd projectname
cordova add platform android
```

adding some plugins

```
cordova plugin add org.apache.cordova.device
cordova plugin add cordova-plugin-network-information
cordova plugin add org.apache.cordova.device
cordova plugin add com.megster.cordova.bluetoothserial
```

running test on real device

android connected via USB and remote USB debugging enabled

`cordova run android`

and then launch chrome browser on desktop host machine 

`chrome://inspect/#`

select the app from list on android device; INSPECT!


## Hardware

### Robotis CM-700

`wget http://www.robotis.com/download/software/CM-9/ROBOTIS_OpenCM-v1.0.2-linux64.tgz`

### Robotis 9.04

Battery must be wired via +/- Vin pins (not the pins labelled with Li-Ion battery)

Dynamixel AX-12A motors connected through 3pin TTL interface

Bluetooth module connect with 4pins

Default settings: 1Mbps, Motor ID 1

`Dxl.begin(3); // set to 1Mbps baud rate` 

Change the motor ID by sending broadcast ID change (one motor at a time)

```
#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04

/* Dynamixel ID defines */
#define NEW_ID 2 //New ID to be changed.

/* Control table defines */
#define ID_Change_Address 1
#define Goal_Postion_Address 30
#define Moving_Speed 32

Dynamixel Dxl(DXL_BUS_SERIAL1);

void setup() {
  // Dynamixel 2.0 Baudrate -> 0: 9600, 1: 57600, 2: 115200, 3: 1Mbps 
  Dxl.begin(3);
  
/*************** CAUTION ***********************
* All dynamixels in bus will be changed to ID 2 by using broadcast ID(0xFE)
* Please check if there is only one dynamixel that you want to change ID
************************************************/
  Dxl.writeByte(BROADCAST_ID, ID_Change_Address, NEW_ID);    //Change current id to new id
  Dxl.jointMode(NEW_ID); //jointMode() to use position mode
}

void loop() {
  /*Turn dynamixel to position 0 by new id*/
  Dxl.writeWord(NEW_ID, Goal_Postion_Address, 0); 
  // Wait for 1 second (1000 milliseconds)
  delay(1000);              
  /*Turn dynamixel to position 300 by new id*/
  Dxl.writeWord(NEW_ID, Goal_Postion_Address, 300);
  // Wait for 1 second (1000 milliseconds)
  delay(1000);              
}
```

