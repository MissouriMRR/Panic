/*
 * 
 */

#include <Servo.h>

typedef unsigned long ulong;

#define SERVO_OFF  1000
#define SERVO_HOVER 1000 // TODO: Find the actual value
#define SERVO_FULL 2000 // TODO: Find the actual value
#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define BACK_LEFT 2
#define BACK_RIGHT 3

Servo rotor[4];
ulong TimeSinceStart;
ulong TimeDelta;
bool ManualControl;


double MicrosToSecond(ulong micro){
  return micro / 1000000.f;
}

void LiftOff(float feet){
  double Height = 0; 
  // Read the height from the ultrasonic sensors 
  for (int i = 0; i < 4; ++i)
    rotor[i].writeMicroseconds(SERVO_HOVER + 250); // Put all four motors on low power, but can still go up.
    
  while (Height < feet)
  {
    Height = feet + 1; // Fill this in with the actual height-finding code later
  }
  for (int i = 0; i < 4; ++i)
    rotor[i].writeMicroseconds(SERVO_HOVER);
}

void Land()
{
  double Height = 0;
  for (int i = 0; i < 4; ++i)
    rotor[i].writeMicroseconds(SERVO_HOVER - 250); // Put all four motors on low power, but can still go up.
    
  while (Height > .5)
  {
    Height = 0; // Fill this in with the actual height-finding code later
  }
  for (int i = 0; i < 4; ++i)
    rotor[i].writeMicroseconds(SERVO_OFF);
}

void setup() {
  TimeSinceStart = millis();
  TimeDelta = 0;
  
  for (int i = 0; i < 4; ++i){
    rotor[i].attach(i); // Set up ESC channels. TODO: Find the offset for i in this case
    rotor[i].writeMicroseconds(SERVO_OFF);
  }
  delay(5000);
  LiftOff(1.5);

  delay(5000);
  Land();
}

void loop(){
  TimeDelta = micros() - TimeSinceStart; 
  TimeSinceStart = micros(); 
}

