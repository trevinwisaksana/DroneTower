/*
 * Sketch to control the pins of Arduino via serial interface
 *
 * Commands implemented with examples:
 *
 * - RD13 -> Reads the Digital input at pin 13
 * - RA4 - > Reads the Analog input at pin 4
 * - WD13:1 -> Writes 1 (HIGH) to digital output pin 13
 * - WA6:125 -> Writes 125 to analog output pin 6 (PWM) 
 */

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Servo.h>

#define TURN_TIME 1 // Turn time determines the number of milliseconds the delay is
#define STOPPED_SERVO 69 // This is the equivilant of 90
#define LOADING_STATE 61 // This is the loading point on the drone
#define UNLOADING_STATE 77 // This is the unloading point to the tower

double angle_rad = PI/180.0; 
double angle_deg = 180.0/PI;

bool is_loading = false;

char operation; // Holds operation (R, W, ...)
char mode; // Holds the mode (D, A)
char tower_state;

int angle = 0; // Used as the angle of the servo
int pin_number; // Holds the pin number
int digital_value; // Holds the digital value
int analog_value; // Holds the analog value
int value_to_write; // Holds the value that we want to write
int wait_for_transmission = 5; // Delay in ms in order to receive the serial data

Servo conveyor_servo;  // Create servo object to control a servo

void set_pin_mode(int pin_number, char mode){
    /*
     * Performs a pinMode() operation depending on the value of the parameter
     * mode :
     * - I: Sets the mode to INPUT
     * - O: Sets the mode to OUTPUT
     * - P: Sets the mode to INPUT_PULLUP
     */

    switch (mode){
        case 'I':
            pinMode(pin_number, INPUT);
            break;
        case 'O':
            pinMode(pin_number, OUTPUT);
            break;
        case 'P':
            pinMode(pin_number, INPUT_PULLUP);
            break;
    }
}

void digital_read(int pin_number){
    /*
     * Performs a digital read on pin_number and returns the value read to serial
     * in this format: D{pin_number}:{value}\n where value can be 0 or 1
     */

    digital_value = digitalRead(pin_number);
    Serial.print('D');
    Serial.print(pin_number);
    Serial.print(':');
    Serial.println(digital_value); // Adds a trailing \n
}

void analog_read(int pin_number){
    /*
     * Performs an analog read on pin_number and returns the value read to serial
     * in this format: A{pin_number}:{value}\n where value ranges from 0 to 1023
     */

    analog_value = analogRead(pin_number);
    Serial.print('A');
    Serial.print(pin_number);
    Serial.print(':');
    Serial.println(analog_value); // Adds a trailing \n
}

void digital_write(int pin_number, int digital_value){
    /*
     * Performs a digital write on pin_number with the digital_value
     * The value must be 1 or 0
     */
  digitalWrite(pin_number, digital_value);
}

void analog_write(int pin_number, int analog_value){
    /*
     * Performs an analog write on pin_number with the analog_value
     * The value must be range from 0 to 255
     */
  analogWrite(pin_number, analog_value);
}

void setup() {
    /*
    Notes: 
    – A serial device is a device that sends bits
    – 
    */
  
    Serial.begin(9600); // Serial Port at 9600 baud: the number of bits per second
    
    Serial.setTimeout(100); // Instead of the default 1000ms, in order
                            // to speed up the Serial.parseInt()
                            
    // Connecting the servo at Pin 6
    conveyor_servo.attach(6);
    conveyor_servo.write(STOPPED_SERVO);
    tower_state = 'S';
}

void controlLoop() { 
    // Reading the command from the Python code 
    // Checking if a character is coming in on the serial port. From the Flask server.
    if (Serial.available() > 0) {
        // operations reads the character that comes in 
        operation = Serial.read();
        
        // The delay is used because the computer might read faster than the character gets
        // This allows abot 5 to 6 characters to show up 
        delay(wait_for_transmission); // If not delayed, second character is not correctly read
        mode = Serial.read();
        pin_number = Serial.parseInt(); // Waits for an int to be transmitted
        
        if (Serial.read()==':') {
            value_to_write = Serial.parseInt(); // Collects the value to be written
        } 
        
        /*Switch statement that has a case for Read, Write, Mode*/
        switch (operation) {
            case 'R': // Read operation, e.g. RD12, RA4
                if (mode == 'D'){ // Digital read
                    digital_read(pin_number);
                } else if (mode == 'A'){ // Analog read
                    analog_read(pin_number);
 
                } else {
                  break; // Unexpected mode
                }
                break;

            case 'W': // Write operation, e.g. WD3:1, WA8:255
                if (mode == 'D'){ // Digital write
                    digital_write(pin_number, value_to_write);
                } else if (mode == 'A'){ // Analog write
                    analog_write(pin_number, value_to_write);
                } else {
                    break; // Unexpected mode
                }
                break;

            case 'M': // Pin mode, e.g. MI3, MO3, MP3
                set_pin_mode(pin_number, mode); // Mode contains I, O or P (INPUT, OUTPUT or PULLUP_INPUT)
                break;

            case 'L': // Loading mode to make the package be loaded
                tower_state = 'L';
                Serial.print('Loading mode engaged');
                break;
                
            case 'U': // Unloading mode to remove the package from the drone
                tower_state = 'U';
                Serial.print('Unloading mode engaged');
                break;
            
            default: // Unexpected char
                break;
        }
    }
}

void loop() {

  switch (tower_state) {
    case 'L':
      for (angle = LOADING_STATE; angle <= 69; angle += 1) {
        Serial.print(angle, DEC);
        Serial.println(" ");
        conveyor_servo.write(angle);
        // Delay is used to prevent the for-loop from being repeated too soon
        _delay(TURN_TIME);
      
        if (angle == 69) {
          tower_state = 'S';
        }
       }
      break;
      
    case 'U':
      for (angle = UNLOADING_STATE; angle >= 69; angle -= 1) {
        Serial.print(angle, DEC);
        Serial.println(" ");
        conveyor_servo.write(angle);
        // Delay is used to prevent the for-loop from being repeated too soon
        _delay(TURN_TIME);
    
        if (angle == 69) {
          tower_state = 'S';
        }
      }
      break;
      
    case 'S':
      conveyor_servo.write(STOPPED_SERVO);
      _delay(TURN_TIME);
      Serial.println("Called");
      break;
    default:
      break;
  }
}

// Custom delay method to be able to intercept the code while on the loop
void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime) {
        controlLoop();
    }
}

