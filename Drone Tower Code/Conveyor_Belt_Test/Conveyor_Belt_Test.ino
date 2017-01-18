#include <Servo.h>
#define TURN_TIME 1000 // Turn time determines the number of milliseconds the delay is
#define STOPPED_SERVO 69 // This is the equivilant of 90
#define LOADING_STATE 61 // This is the loading point on the drone
#define UNLOADING_STATE 77 // This is the unloading point to the tower

Servo servo_test;

int angle = 0; // Used as the angle of the servo
bool is_loading = true; // Used to check if the motor is loading

void setup()
{
  Serial.begin(9600);
  servo_test.attach(6);
  servo_test.write(STOPPED_SERVO);
}

void loop()
{

  if (is_loading == true) {
    for (angle = LOADING_STATE; angle <= 69; angle += 1)
    {
      Serial.print(angle, DEC);
      Serial.println(" ");
      servo_test.write(angle);
      delay(TURN_TIME); // Delay is used to prevent the for-loop from being repeated too soon
  
      if (angle == 69) {
        is_loading = false;
      }
    }
  }

  if (is_loading == false) {
    for (angle = UNLOADING_STATE; angle >= 69; angle -= 1)
    {
      Serial.print(angle, DEC);
      Serial.println(" ");
      servo_test.write(angle);
      delay(TURN_TIME); // Delay is used to prevent the for-loop from being repeated too soon
  
      if (angle == 69) {
        is_loading = true;
      }
    }
  }
  
}


