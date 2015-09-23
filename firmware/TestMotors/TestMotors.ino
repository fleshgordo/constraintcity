/* Test all three motors with ID 1, 2, 3
  
*/
/* Serial device defines for dxl bus */
#define DXL_BUS_SERIAL1 1  //Dynamixel on Serial1(USART1)  <-OpenCM9.04

/* Dynamixel ID defines */
#define motor1 1
#define motor2 2
#define motor3 3

int clockAngleLimit = 480;
int cclockAngleLimit = 840;

Dynamixel Dxl(DXL_BUS_SERIAL1);

void setup() {
  Dxl.begin(3);
  Dxl.jointMode(motor1); //jointMode() is to use position mode
  Dxl.jointMode(motor2);
  Dxl.jointMode(motor3);
  Dxl.cwAngleLimit(motor1, clockAngleLimit);   //CW Angle Limit -> minimum 300
  Dxl.ccwAngleLimit(motor1, cclockAngleLimit);  //CCW Angle Limit -> minimum 700  
  Dxl.cwAngleLimit(motor2, clockAngleLimit);   //CW Angle Limit -> minimum 300
  Dxl.ccwAngleLimit(motor2, cclockAngleLimit);  //CCW Angle Limit -> minimum 700  
  Dxl.cwAngleLimit(motor3, clockAngleLimit);   //CW Angle Limit -> minimum 300
  Dxl.ccwAngleLimit(motor3, cclockAngleLimit);  //CCW Angle Limit -> minimum 700  
  Dxl.goalSpeed(motor1, 1000);
  Dxl.goalSpeed(motor2, 1000);
  Dxl.goalSpeed(motor3, 1000);
}

void loop() {
  SerialUSB.println("moving motors to pos: 1");
  Dxl.goalPosition(motor1,1);
  Dxl.goalPosition(motor2,1);
  Dxl.goalPosition(motor3,1);
  delay(5000); 
  SerialUSB.println("moving motors to pos: 1023");
  Dxl.goalPosition(motor1,1023);
  Dxl.goalPosition(motor2,1023);
  Dxl.goalPosition(motor3,1023);
  delay(5000);
}

