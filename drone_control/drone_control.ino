/*
 * 
 */

#include <NewPing.h>
#include <Adafruit_L3GD20.h>
#include <Servo.h>
#include <PID_v1.h>
#include "drone_common.h"

typedef unsigned long ulong;

#define SERVO_OFF  1000
#define SERVO_HOVER 1200 // TODO: Find the actual value
#define SERVO_FULL 2100 // TODO: Find the actual value

#define ECHO_PIN 10
#define TRIGGER_PIN 11
#define MAX_HEIGHT 100

#define MOTOR_PIN_OFFSET 2
#define FRONT_LEFT  0
#define FRONT_RIGHT 1
#define BACK_LEFT   2
#define BACK_RIGHT  3

#define TEST_CONTROLS

struct Angles
{
  double roll;
  double pitch;
  double yaw;
};

Angles CurrentAngle;

Servo rotor[4];
ulong TimeSinceStart;
ulong TimeDelta;
bool ManualControl = true;

NewPing sonar(ECHO_PIN, TRIGGER_PIN, MAX_HEIGHT);

double SetYaw,   InputYaw,   OutputYaw;
double SetRoll,  InputRoll,  OutputRoll;
double SetPitch, InputPitch, OutputPitch;

PID YawPid  (&InputYaw,   &OutputYaw,   &SetYaw,   5, 10, 10, DIRECT);
PID RollPid (&InputRoll,  &OutputRoll,  &SetRoll,  5, 10, 10, DIRECT);
PID PitchPid(&InputPitch, &OutputPitch, &SetPitch, 5, 10, 10, DIRECT);

Adafruit_L3GD20 gyro;

float GetHeight()
{
  return sonar.ping_in();
}

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

void FlyForTime(int motorValue, int timePeriod)
{
  static int flightStartTime = millis();
  static bool isFlying = false;
  
  if(TimeDelta - flightStartTime < timePeriod && !isFlying)
  {
    SetMotor(FRONT_LEFT, SERVO_HOVER);
    SetMotor(FRONT_RIGHT, SERVO_HOVER);

    SetMotor(BACK_LEFT, SERVO_HOVER + 250);
    SetMotor(BACK_RIGHT, SERVO_HOVER + 250);
      
    isFlying = true;
  }
  else if(TimeDelta - flightStartTime >= timePeriod && isFlying)
  {
    SetMotor(BACK_LEFT, SERVO_HOVER);
    SetMotor(BACK_RIGHT, SERVO_HOVER);
    flightStartTime = 0;
    isFlying = false;
  }  
}

void FlyForDistance(int motorValue, float distance)
{
  
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
  CurrentAngle = {};
  Serial.begin(115200);

  /*set up PID */
  YawPid.SetOutputLimits(SERVO_OFF, SERVO_FULL); 
  PitchPid.SetOutputLimits(SERVO_OFF, SERVO_FULL);
  RollPid.SetOutputLimits(SERVO_OFF, SERVO_FULL);
  /*end set up PID*/
  
#ifdef TEST_CONTROLS
  ManualControl = true;
#else
  ManualControl = false;
#endif

  TimeSinceStart = millis();
  TimeDelta = 0;
  
  for (int i = FRONT_LEFT; i <= BACK_RIGHT; ++i){
    rotor[i].attach(i + MOTOR_PIN_OFFSET); // Set up ESC channels. TODO: Find the offset for i in this case
    SetMotor(i, SERVO_OFF);
  }

  if(!gyro.begin(gyro.L3DS20_RANGE_250DPS))
  {
    Serial.println("Failed to initialize Gyro unit");
    while(true); // Loop forever after failure.
  }

  delay(5000); // Delay for the battery
}


ControllerInput GetControllerInput()
{
  ControllerInput Result;

  const short CONTROL_THROTTLE = 6;
  const short CONTROL_PITCH    = 7;
  const short CONTROL_YAW      = 8;
  const short CONTROL_ROLL     = 9;
  
  Result.Throttle = pulseIn(CONTROL_THROTTLE, HIGH, 25000);
  Result.Pitch    = pulseIn(CONTROL_PITCH, HIGH, 25000);
  Result.Yaw      = pulseIn(CONTROL_YAW, HIGH, 25000);
  Result.Roll     = pulseIn(CONTROL_ROLL, HIGH, 25000);

  return Result;
}


void loop(){
  TimeDelta = micros() - TimeSinceStart; 
  TimeSinceStart = micros(); 

  gyro.read();

  CurrentAngle.roll  += gyro.data.y * MicrosToSecond(TimeDelta);
  CurrentAngle.pitch += gyro.data.x * MicrosToSecond(TimeDelta); // This method has a LOT of error.
  CurrentAngle.yaw   += gyro.data.z * MicrosToSecond(TimeDelta); // But it's honestly the best I can do

  if (ManualControl)
  {
    const int CONTROLLER_MAX = 1024;
    const int CONTROLLER_MIN = 0;

    ControllerInput Input = GetControllerInput();
    int Throttle = Input.Throttle;
    SetRoll      = Input.Roll;
    SetPitch     = Input.Pitch;
    SetYaw       = Input.Yaw;

    // Since the angle is in the range (0, 360), we need to clamp the Set* values to that
    SetPitch = map(SetPitch, CONTROLLER_MIN, CONTROLLER_MAX, 0, 360);
    SetPitch = map(SetPitch, CONTROLLER_MIN, CONTROLLER_MAX, 0, 360);
    SetPitch = map(SetPitch, CONTROLLER_MIN, CONTROLLER_MAX, 0, 360);

    InputPitch = CurrentAngle.pitch;
    InputRoll  = CurrentAngle.roll;
    InputYaw   = CurrentAngle.yaw;

    YawPid.Compute();
    PitchPid.Compute();
    RollPid.Compute();

    int Yaw   = OutputYaw;
    int Roll  = OutputRoll;
    int Pitch = OutputPitch;
    
    Throttle = map(Throttle, CONTROLLER_MIN, CONTROLLER_MAX, SERVO_OFF, SERVO_FULL);

    SetMotor(FRONT_LEFT,  +Yaw + Roll - Pitch + Throttle);
    SetMotor(FRONT_RIGHT, -Yaw - Roll - Pitch + Throttle);
    SetMotor(BACK_LEFT,   -Yaw + Roll + Pitch + Throttle);
    SetMotor(BACK_RIGHT,  +Yaw - Roll + Pitch + Throttle);

  }
  else
  {
    // Actual autonomous code goes here...
  }

  delay(10);
}

