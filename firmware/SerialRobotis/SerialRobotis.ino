/* Serial device defines for dxl bus */
#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04

Dynamixel Dxl(DXL_BUS_SERIAL1);

/* Globals */
boolean toogleM = false;
int incomingByte = 0;
int incomingArray[3];
int counter = 0;
int motorMultiplier = 1;

/* Globals for motors */
int clockWiseAngleLimit = 480;
int cclockWiseAngleLimit = 840;
int motorGoalSpeed = 1000;

int motor1Pos = 1023;
int motor2Pos = 1023;
int motor3Pos = 1023;

#define motorID1 1
#define motorID2 2
#define motorID3 3

/* General setup */
void setup(){
   
  // Setup motors
  
  // Dynamixel baud rate 3 eq. 1Mbps 
  Dxl.begin(3);  
  
  Dxl.jointMode(motorID1); //jointMode() is to use position mode
  Dxl.jointMode(motorID2);
  Dxl.jointMode(motorID3);
 
  Dxl.cwAngleLimit(motorID1, clockWiseAngleLimit);   //CW Angle Limit -> minimum 300
  Dxl.ccwAngleLimit(motorID1, cclockWiseAngleLimit);  //CCW Angle Limit -> minimum 700  
  Dxl.cwAngleLimit(motorID2, clockWiseAngleLimit);   //CW Angle Limit -> minimum 300
  Dxl.ccwAngleLimit(motorID2, cclockWiseAngleLimit);  //CCW Angle Limit -> minimum 700  
  Dxl.cwAngleLimit(motorID3, clockWiseAngleLimit);   //CW Angle Limit -> minimum 300
  Dxl.ccwAngleLimit(motorID3, cclockWiseAngleLimit);  //CCW Angle Limit -> minimum 700  
  
  Dxl.goalSpeed(motorID1, motorGoalSpeed);
  Dxl.goalSpeed(motorID2, motorGoalSpeed);
  Dxl.goalSpeed(motorID3, motorGoalSpeed);
  
  pinMode(BOARD_LED_PIN, OUTPUT);
  
  //Serial2 Serial initialize
  Serial2.begin(9600);  
  delay(2000);
  SerialUSB.println("Serial port2 initialised");
}

/* 
  LOOOOOOOOP 
*/
void loop(){
  while (Serial2.available()) {
    
    // Read from serial port
    incomingByte = Serial2.read();
    incomingArray[counter] = incomingByte;
    counter++;
      
    // Set motors
    if ((char)incomingByte == '\n') {
      SerialUSB.print("toggle");
      SerialUSB.println(toogleM);
      if (toogleM == true) {
        motor1Pos = incomingArray[0];
        motor2Pos = incomingArray[1];
        motor3Pos = incomingArray[2];
        moveMotors();
      }
      else {
        releaseMotors();
      }
      counter = 0;
      cleanArray(incomingArray);
    }
    
    // Activate motors
    if ((char)incomingByte == 'a') {
      SerialUSB.println("activate motors");
      toogleM = !toogleM;
      counter = 0;
      cleanArray(incomingArray);
    }
    
    // Disactivate motors
    else if ((char)incomingByte == 'd') { 
      SerialUSB.println("disactivate motors");
      releaseMotors();
      toogleM =  false;
      counter = 0;
      cleanArray(incomingArray);
    }
    
    // Increase the motor multiplier
    else if ((char)incomingByte == 'm') { 
      if (motorMultiplier < 15) {
        motorMultiplier += 1;
        SerialUSB.print("increasing pain to: ");
        SerialUSB.println(motorMultiplier);
      }
      counter = 0;
      cleanArray(incomingArray);
    }
   
       // Decrease the motor multiplier
    else if ((char)incomingByte == 'l') { 
      if (motorMultiplier > 0) {
        motorMultiplier -= 1;
        SerialUSB.print("lowering pain to: ");
        SerialUSB.println(motorMultiplier);
      }
      counter = 0;
      cleanArray(incomingArray);
    } 
    // test the motors
    else if ((char)incomingByte == 't') { 
      toogleM =  false;
      testMotors();
      counter = 0;
      cleanArray(incomingArray);
    }
    
     // Check the motors
    else if ((char)incomingByte == 'c') { 
      toogleM =  false;
      checkMotor();
      counter = 0;
      cleanArray(incomingArray);
    }
  }
}

/* 
  Function that checks motor
*/
void checkMotor() {
  for (int i=1;i<4;i++) {
    Dxl.ping(i);
    if( Dxl.getResult() == COMM_RXSUCCESS )  {
      SerialUSB.print("Motor ");
      SerialUSB.print(i);
      SerialUSB.println(" ok");
    }
    else {
      SerialUSB.print("Motor ");
      SerialUSB.print(i);
      SerialUSB.println(" problem");
    }
  }
}

/* 
  Cleaning an array (that was defined globally)
  @param {array}  array - empty an array
*/
void cleanArray(int* array) {
  for( int i = 0; i < sizeof(array);  i++ ) {
    array[i] = 0;
  }
}

/* 
  Move motors to their position
*/
void moveMotors() {
  int motor1Scaled = 1023 - (motor1Pos * motorMultiplier);
  int motor2Scaled = 1023 - (motor2Pos * motorMultiplier);
  int motor3Scaled = 1023 - (motor3Pos * motorMultiplier);
  
  SerialUSB.println("move motors");
  SerialUSB.print("pain multiplier: ");
  SerialUSB.println(motorMultiplier);
  SerialUSB.print("motor1: ");
  SerialUSB.println(motor1Scaled);
  SerialUSB.print("motor2: ");
  SerialUSB.println(motor2Scaled);
  SerialUSB.print("motor3: ");
  SerialUSB.println(motor3Scaled);
  SerialUSB.println("----");
  
  Dxl.goalPosition(motorID1,motor1Scaled);
  Dxl.goalPosition(motorID2,motor2Scaled);
  Dxl.goalPosition(motorID3,motor3Scaled);
}

/* 
  Release all motors (go to target 0 pos)
*/
void releaseMotors() {
  SerialUSB.println("release motors");
  Dxl.goalPosition(motorID1,1023);
  Dxl.goalPosition(motorID2,1023);
  Dxl.goalPosition(motorID3,1023);
}
 
void testMotors() {
  SerialUSB.println("moving motors to pos: 1");
  Dxl.goalPosition(motorID1,1);
  Dxl.goalPosition(motorID2,1);
  Dxl.goalPosition(motorID3,1);
  delay(1000); 
  SerialUSB.println("moving motors to pos: 1023");
  Dxl.goalPosition(motorID1,1023);
  Dxl.goalPosition(motorID2,1023);
  Dxl.goalPosition(motorID3,1023);
  delay(1000);
}
