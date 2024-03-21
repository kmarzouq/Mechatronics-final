#include <Servo.h>
Servo myservo;  // create servo object to control a servo
int val = 0;

int potpin = A0;  // analog pin used to connect the potentiometer

void servo_setup() {
  myservo.attach(6);  // attaches the servo on pin 9 to the servo object
  // myservo.write(0);                  // sets the servo position according to the scaled value
  // delay(1000);
  // myservo.write(82);
}
void hit() {
  myservo.write(0);
  delay(1000); 
  myservo.write(80);
  delay(500);
  myservo.write(0);
}


  // void loop() {
  //   if (val <= 120) val++;
  //              // reads the value of the potentiometer (value between 0 and 1023)
  //   val = map(val, 0, 1023, 0, 120);     // scale it for use with the servo (value between 0 and 120)
  //   myservo.write(val);                  // sets the servo position according to the scaled value
  //   delay(10);                           // waits for the servo to get there
  // }
