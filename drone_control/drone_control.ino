/*
 * 
 */
#include <Adafruit_L3GD20.h>
#include <Servo.h>
#include "drone_common.h"

typedef unsigned long ulong;

#define SERVO_OFF  1000
#define SERVO_HOVER 1000 // TODO: Find the actual value
#define SERVO_FULL 3000 // TODO: Find the actual value


#define FRONT_LEFT  2
#define FRONT_RIGHT 3
#define BACK_LEFT   4
#define BACK_RIGHT  5

#define TEST_CONTROLS

Servo rotor[4];
ulong TimeSinceStart;
ulong TimeDelta;
bool ManualControl;

Adafruit_L3GD20 gyro;

void SetMotor(int MotorID, int value)
{
  rotor[MotorID].writeMicroseconds(value);
}

void SetAllMotors(int value)
{
  for (int i = FRONT_LEFT; i <= BACK_RIGHT; ++i)
    SetMotor(i, value);
}

double MicrosToSecond(ulong micro){
  return micro / 1000000.f;
}

void LiftOff(float feet){
  double Height = 0; 
  // Read the height from the ultrasonic sensors 
  SetAllMotors(SERVO_HOVER + 250); // Put all four motors on low power, but can still go up.
    
  while (Height < feet)
  {
    Height = feet + 1; // Fill this in with the actual height-finding code later
  }
  for (int i = 0; i < 4; ++i)
    SetMotor(i, SERVO_HOVER);
}

void Land()
{
  double Height = 0;
  SetAllMotors(SERVO_HOVER - 250); // Put all four motors on low power, but can still go up.
    
  while (Height > .5)
  {
    Height = 0; // Fill this in with the actual height-finding code later
  }
  SetAllMotors(SERVO_OFF);
}

void setup() {
  Serial.begin(9600);
#ifdef TEST_CONTROLS
  ManualControl = true;
#else
  ManualControl = false;
#endif

  TimeSinceStart = millis();
  TimeDelta = 0;
  
  for (int i = FRONT_LEFT; i <= BACK_RIGHT; ++i){
    rotor[i].attach(i); // Set up ESC channels. TODO: Find the offset for i in this case
    SetMotor(i, SERVO_OFF);
  }

  gyro.begin(gyro.L3DS20_RANGE_250DPS);

  delay(5000); // Delay for the battery
}


ControllerInput GetControllerInput()
{
  ControllerInput Result;

  const short CONTROL_THROTTLE = 6;
  const short CONTROL_PITCH    = 7;
  const short CONTROL_YAW      = 8;
  const short CONTROL_ROLL     = 9;
  
  Result.Throttle = analogRead(CONTROL_THROTTLE);
  Result.Pitch = analogRead(CONTROL_PITCH);
  Result.Yaw = analogRead(CONTROL_YAW);
  Result.Roll = analogRead(CONTROL_ROLL);

  return Result;
}


void loop(){
  gyro.read();

  if (ManualControl)
  {
    const int CONTROLLER_MAX = 1024;
    const int CONTROLLER_MIN = 0;

    ControllerInput Input = GetControllerInput();
    int Throttle = Input.Throttle;
    int Roll     = Input.Roll;
    int Pitch    = Input.Pitch;
    int Yaw      = Input.Yaw;
    
    Throttle = map(Throttle, CONTROLLER_MIN, CONTROLLER_MAX, SERVO_OFF, SERVO_FULL);
    Roll     = map(Roll,     CONTROLLER_MIN, CONTROLLER_MAX, SERVO_OFF, SERVO_FULL);
    Pitch    = map(Pitch,    CONTROLLER_MIN, CONTROLLER_MAX, SERVO_OFF, SERVO_FULL);
    Yaw      = map(Yaw,      CONTROLLER_MIN, CONTROLLER_MAX, SERVO_OFF, SERVO_FULL);

    SetMotor(FRONT_LEFT,  +Yaw + Roll - Pitch + Throttle);
    SetMotor(FRONT_RIGHT, -Yaw - Roll - Pitch + Throttle);
    SetMotor(BACK_LEFT,   -Yaw + Roll + Pitch + Throttle);
    SetMotor(BACK_RIGHT,  +Yaw - Roll + Pitch + Throttle);

  }
  else
  {
    // Actual autonomous code goes here...
  }

  TimeDelta = micros() - TimeSinceStart; 
  TimeSinceStart = micros(); 
}

