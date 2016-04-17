/*
 * 
 */

#include <Servo.h>
#include "drone_common.h"

typedef unsigned long ulong;

#define SERVO_OFF  1000
#define SERVO_HOVER 1000 // TODO: Find the actual value
#define SERVO_FULL 2000 // TODO: Find the actual value
#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define BACK_LEFT 2
#define BACK_RIGHT 3
#define TEST_CONTROLS

Servo rotor[4];
ulong TimeSinceStart;
ulong TimeDelta;
bool ManualControl;

void SetMotor(int MotorID, int value)
{
  rotor[MotorID].writeMicroseconds(value);
}

double MicrosToSecond(ulong micro){
  return micro / 1000000.f;
}

void LiftOff(float feet){
  double Height = 0; 
  // Read the height from the ultrasonic sensors 
  for (int i = 0; i < 4; ++i)
    SetMotor(i, SERVO_HOVER + 250); // Put all four motors on low power, but can still go up.
    
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
  for (int i = 0; i < 4; ++i)
    SetMotor(i, SERVO_HOVER - 250); // Put all four motors on low power, but can still go up.
    
  while (Height > .5)
  {
    Height = 0; // Fill this in with the actual height-finding code later
  }
  for (int i = 0; i < 4; ++i)
    SetMotor(i, SERVO_OFF);
}

void setup() {
#ifdef TEST_CONTROLS
  ManualControl = true;
#else
  ManualControl = false;
#endif

  TimeSinceStart = millis();
  TimeDelta = 0;
  
  for (int i = 0; i < 4; ++i){
    rotor[i].attach(i); // Set up ESC channels. TODO: Find the offset for i in this case
    SetMotor(i, SERVO_OFF);
  }
  delay(5000);
  LiftOff(1.5);

  delay(5000);
  Land();
}


ControllerInput GetControllerInput()
{
  // TODO: This function
  ControllerInput Result;

  return Result;
}


void loop(){
  if (ManualControl)
  {
    ControllerInput Input = GetControllerInput();
    int Throttle = Input.Throttle;
    int Roll     = Input.Roll;
    int Pitch    = Input.Pitch;
    int Yaw      = Input.Yaw;

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

