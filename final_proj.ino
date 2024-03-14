// #include <TimerOne.h>
// #include <QTRSensors.h>

// QTRSensors qtr;

// int thresh;
// int readl[8];
// volatile int target = 0;
// volatile int count = 0;
// volatile int signal_length = 14;
// volatile int signal_counter = 0;

// volatile bool enA = false;
// volatile bool in1 = false;
// volatile bool in2 = false;

// volatile bool enB = false;
// volatile bool in1B = false;
// volatile bool in2B = false;

// volatile bool move = false;
// volatile int pstage = 0;
// volatile int time;
// volatile int d_count = 0;
// volatile bool calibrated = false;

// void setup() {
//   Serial.begin(9600);

//   pinMode(44, OUTPUT);
//   pinMode(45, OUTPUT);
//   pinMode(46, OUTPUT);
//   pinMode(47, OUTPUT);
//   pinMode(48, OUTPUT);
//   pinMode(49, OUTPUT);
//   pinMode(50, OUTPUT);
//   pinMode(51, OUTPUT);

//   pinMode(8, OUTPUT);
//   pinMode(9, OUTPUT);
//   pinMode(10, OUTPUT);

//   pinMode(11, OUTPUT);
//   pinMode(12, OUTPUT);
//   pinMode(13, OUTPUT);

//   pinMode(38, INPUT);
//   attachInterrupt(digitalPinToInterrupt(38), calibrate, CHANGE);
  
//   forward();
//   Timer1.initialize(2000);
//   Timer1.attachInterrupt(signal);
//   coast();
 
//   qtr.setTypeRC();
//   qtr.setSensorPins((const uint8_t[]){30, 28, 26, 24, 31, 29, 27, 25}, 8);


//   // Serial.println("ready for b");
//   // // delay(5000);
//   // bool f1 = false;
//   // while(!f1){
//   //   if(Serial.available()){
//   //     f1 = true;
//   //     while(Serial.read()!= '\n') {}
//   //     Serial.println("calibrating");
//   //   }
//   // }
  
// }

// int pos;
// long senstime = 0;
// const int middle = 4500;
// int direction = 0;
// int directionInt = 0;
// long lastmeas = 0;
// int directions[] = {0, 0, 0, 0, 0};
// int directionsi = 0;
// void loop() {
//   if (!calibrated) return;
//   if(millis() - lastmeas > 50) {
//     lastmeas = millis();
//     pos = qtr.readLineBlack(readl);

//     // for (int i=0; i<8; i++){
//     //   Serial.print(readl[i]);
//     //   Serial.print(" ");
//     // }

//     // proportional
//     direction = pos - middle;
//     Serial.println(direction);

//     // Integral
//     directionsi = (directionsi + 1) % 5;
//     directions[directionsi] = direction;
//     int sum = 0;
//     for (int i = 0; i<5; i++){
//       sum += directions[i];
//     }
//     // Serial.println(sum);

    
//     if (direction > 2500) {
//       speed(6);
//       Serial.println("rot_left");
//       pivotLeft();
//     } else if (direction > 1000)  {
//       speed(8);
//       Serial.println("turn left");
//       turnLeft();
//     } else if (direction < -2500) {
//       speed(6);
//       Serial.println("rot_right");
//       pivotRight();
//     } else if (direction < -1000) {
//       speed(8);
//       Serial.println("turn right");
//       turnRight();
//     } else {
//       speed(10);
//       Serial.println("straight");
//       forward();
//     }
//   }
// }

// void calibrate() {
//   for (int i = 0; i < 250; i++) {
//     qtr.calibrate();
//     delay(20);
//   }

//   qtr.readCalibrated(readl);
//   for (int i=0; i<8; i++){
//     Serial.print(readl[i]);
//     Serial.print(" ");
//   }
//   Serial.println(" ");

//   Serial.println("ready for read");
//   calibrated = true;
// }

// void forward() {
//  enA = true;
//  in1 = true;
//  in2 = false;

//  enB = true;
//  in1B = true;
//  in2B = false;
//  set_pins();
// }


// void reverse(){
//  enA = true;
//  in1 = false;
//  in2 = true;

//  enB = true;
//  in1B = false;
//  in2B = true;
//  set_pins();
// }


// void brake(){
//  enA = true;
//  in1 = false;
//  in2 = false;

//  enB = true;
//  in1B = false;
//  in2B = false;
//  set_pins();
// }


// void coast(){
//  enA = false;
//  in1 = false;
//  in2 = false;

//  enB = false;
//  in1B = false;
//  in2B = false;
//  set_pins();
// }


// void turnLeft() {
//  enA = false;

//  enB = true;
//  in1B = true;
//  in2B = false;
//  set_pins();
// }
// void turnRight() {
//  enA = true;
//  in1 = true;
//  in2 = false;

//  enB = false;
//  set_pins();
// }
// void pivotLeft() {
//  enA = true;
//  in1 = false;
//  in2 = true;

//  enB = true;
//  in1B = true;
//  in2B = false;
//  set_pins();
// }
// void pivotRight() {
//  enA = true;
//  in1 = true;
//  in2 = false;

//  enB = true;
//  in1B = false;
//  in2B = true;
//  set_pins();
// }

// void measure() {
//   d_count++;
//   if (move) {count++;
//   Serial.println(count);}
//   if (move && count > target) {
//     // Serial.println()
//     move = false;
//     brake();
//   }
// }

// void cmForward(float x){
//   count = 0;
//   target = x*90 - 100; // -100 for braking time
//   move = true;
//   forward();
// }

// void cmReverse(float x) {
//  count = 0;
//  target = x*90 - 100; // -100 for braking time
//  move = true;
//  reverse();
// }

// void speed(int s) {
//   signal_length = s*2;
// }

// void signal() {
//   signal_counter++;
//   if (signal_counter == 20) {
//     digitalWrite(13, enA);
//     digitalWrite(8, enB);
//     signal_counter = 0;
//   }
//   if (signal_counter >= signal_length) {
//     digitalWrite(8, LOW);
//     digitalWrite(13, LOW);
//   }
// }

// void set_pins() {
//   digitalWrite(13, LOW);
//   digitalWrite(8, LOW);
//   digitalWrite(12, in1);
//   digitalWrite(11, in2);
//   digitalWrite(9, in1B);
//   digitalWrite(10, in2B);
//   digitalWrite(13, enA);
//   digitalWrite(8, enB);
// }