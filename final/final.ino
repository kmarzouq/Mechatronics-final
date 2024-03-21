#include <TimerOne.h>
#include <QTRSensors.h>
#include <Servo.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <stdbool.h>
#include <stdlib.h>

Servo myservo;
QTRSensors qtr;

int thresh;
int readl[8];
int button=0;
int buttonp=0;
void buttonpress(){
 //add counter that increments +1
 button += 1;
}


volatile int turn=0;
volatile int target = 0;
volatile int count = 0;
volatile int signal_length = 14;
volatile int signal_counter = 0;
volatile int last_mov=0;
volatile bool enA = false;
volatile bool in1 = false;
volatile bool in2 = false;

volatile bool enB = false;
volatile bool in1B = false;
volatile bool in2B = false;

volatile bool move = false;
volatile int pstage = 0;
volatile int time;
volatile int d_count = 0;
const int tpc = 86; // ticks/cm for wheels
volatile bool calibrated = false;
void distance_setup() {
  pinMode(A0, INPUT);
}

void hit() {
  myservo.write(0);
  delay(1000);
  myservo.write(120);
  delay(500);
  myservo.write(0);
}

int anim[] = {32,33,34,35,36,37,38,39};
float points[] = {3.16, 2.51, 1.73, 1.3, 1.05, 0.83, 0.6, 0.41};
float avg_slope = -0.0597;
// float points[] = {660, 519, 340, 290, 240, 210, 195, 175};

// float avg_slope = -14.343;

int countd = 0;
float total = 0;
float distances[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int di = 0;
const int SAMPLES = 10;
const byte pinLED = A0;
// todo: points were innacurate in maker lab, account for lighting conditions?
int measure_distance() {
 float volts = analogRead(A0)/1024.0 * 5;
   //Serial.println(volts);
   int i = 0;
   while(volts <= points[i] && i <=8 ) i++;
   //Serial.println(i);


   float slope;
   if (i<=0){
    //Serial.println(5 - (points[0]- volts)/ avg_slope);
    return (5 - (points[0]- volts)/ avg_slope);
   } else {
   if (i >= 8 )
     slope = avg_slope;
   else
     slope = (points[i] - points[i-1])/5;
   }

   //Serial.println((volts - points[i-1])/ slope + i*5);
   return (volts - points[i-1])/ slope + i*5 ;
}

volatile long int stsw;
void setup() {

  Serial.begin(9600);
  pinMode(pinLED, OUTPUT);

  distance_setup();
  Serial.println(measure_distance());

  attachInterrupt(digitalPinToInterrupt(2), measure, CHANGE);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3),buttonpress, FALLING);
  for(int h=0;h<8;h++){
    pinMode(anim[h],OUTPUT);
    //digitalWrite(anim[h],HIGH);

  }
  digitalWrite(anim[7],HIGH);

  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(48, OUTPUT);
  pinMode(49, OUTPUT);
  pinMode(50, OUTPUT);
  pinMode(51, OUTPUT);
  pinMode(53, OUTPUT);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  //pinMode(38, INPUT);
  //attachInterrupt(digitalPinToInterrupt(38), calibrate, CHANGE);
  brake();
  // forward();
  Timer1.initialize(2000);
  Timer1.attachInterrupt(signal);


  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){30, 28, 26, 24, 31, 29, 27, 25}, 8);
  // qtr.setSensorPins((const uint8_t[]){24, 25, 26, 27, 28, 29, 30, 31}, 8);


  Serial.println("waiting to calibrate");
  // delay(5000);
  bool f1 = false;
  volatile int wait;
  stsw=millis();
  //Serial.println(stsw);
  //Serial.println(millis());
  while(button==buttonp){
    //Serial.println(stsw);
    if(millis()-stsw > 1000){
      wait=(wait+1) % 8;
      stsw = millis();
      for(int lp12 = 0; lp12<10;lp12++){
        if(lp12==wait){
          digitalWrite(anim[lp12],HIGH);
        }
        else{
          digitalWrite(anim[lp12],LOW);

        }
      }
    }
  }
  //while(button){
    // if(true){
    //   f1 = true;

  Serial.println("calibrate 1");
  stsw = millis();
  for (int i = 0; i < 100; i++) {
    qtr.calibrate();
    delay(20);
    if(millis()-stsw > 200){
      wait=(wait+1) % 8;
      stsw = millis();
      for(int lp12 = 0; lp12<10;lp12++){
        if(lp12==wait){
          digitalWrite(anim[lp12],HIGH);
        }
        else{
          digitalWrite(anim[lp12],LOW);

        }
      }
    }
  }
  buttonp=button;
  Serial.println("waiting to calibrate 2");
  stsw = millis();
  while(button==buttonp){
    //Serial.println(stsw);
    if(millis()-stsw > 1000){
      wait=(wait-1+8) % 8;
      stsw = millis();
      for(int lp12 = 0; lp12<10;lp12++){
        if(lp12==wait){
          digitalWrite(anim[lp12],HIGH);
        }
        else{
          digitalWrite(anim[lp12],LOW);

        }
      }
    }
  }

  Serial.println("calibrate 2");
  stsw = millis();
  for (int i = 0; i < 100; i++) {
    qtr.calibrate();
    delay(20);
    if(millis()-stsw > 200){
      wait=(wait-1+8) % 8;
      stsw = millis();
      for(int lp12 = 0; lp12<10;lp12++){
        if(lp12==wait){
          digitalWrite(anim[lp12],HIGH);
        }
        else{
          digitalWrite(anim[lp12],LOW);

        }
      }
    }
  }
  buttonp=button;
  qtr.readCalibrated(readl);

  Serial.println(" ");

  Serial.println("ready");


  myservo.attach(6);
  hit();
  stsw=millis();
  wait=0;
  while(button==buttonp){
    //Serial.println(stsw);
    if(millis()-stsw > 1000){
      wait= ~wait;
      stsw = millis();
      for(int lp12 = 0; lp12<10;lp12++){

        if((lp12-wait+8)%2==0){
          digitalWrite(anim[lp12],HIGH);
        }
        else{
          digitalWrite(anim[lp12],LOW);

        }

      }
    }
  }
  buttonp=button;
  for(int lp12 = 0; lp12<10;lp12++){

        digitalWrite(anim[lp12],LOW);

      }
  // cmForward(10);
  // delay(1000);
}

int pos;
long senstime = 0;
const int middle = 4000;
int direction = 0;
int directionInt = 0;
long lastmeas = 0;
int directions[] = {0, 0, 0, 0, 0};
int directionsi = 0;
int w_count = 0;
int col_loc[] = {0, 6, 1, 4, 5, 0, 2};
long etime = 0;


void follow() {
  if(millis() - lastmeas > 20) {
    lastmeas = millis();
    pos = qtr.readLineBlack(readl);

    // for (int i=0; i<8; i++){
    //   Serial.print(readl[i]);
    //   Serial.print(" ");
    // }

    // proportional
    direction = pos - middle;
    //Serial.println(direction);

    // Integral
    directionsi = (directionsi + 1) % 5;
    directions[directionsi] = direction;
    int sum = 0;
    for (int i = 0; i<5; i++){
      sum += directions[i];
    }
    // Serial.println(sum);


    if(direction > 2700) {
      speed(4);
      // Serial.println("rot_left");
      pivotLeft();
    } else if (direction > 800)  {
      speed(6);
      // Serial.println("turn left");
      turnLeft();
    } else if (direction < -2700) {
      speed(4);
      // Serial.println("rot_right");
      pivotRight();
    } else if (direction < -800) {
      speed(6);
      // Serial.println("turn right");
      turnRight();
    } else {
      speed(8);
      // Serial.println("straight");
      forward();
      last_mov=0;
    }
  }

}
void followR(){

  if(millis() - lastmeas > 50) {
    lastmeas = millis();
    pos = qtr.readLineBlack(readl);

    // for (int i=0; i<8; i++){
    //   Serial.print(readl[i]);
    //   Serial.print(" ");
    // }

    // proportional
    direction = pos - middle;
    //Serial.println(direction);

    // Integral
    directionsi = (directionsi + 1) % 5;
    directions[directionsi] = direction;
    int sum = 0;
    for (int i = 0; i<5; i++){
      sum += directions[i];
    }
    // Serial.println(sum);


    if(direction > 2500) {
      speed(2);
      //Serial.println("rot_left");
      pivotRight();
      }
      else if (direction < -2500) {
      speed(2);
     // Serial.println("rot_right");
      pivotLeft();
    }  else {
      speed(2);
      //Serial.println("straight");
      reverse();
    }
  }

}
int state=0;

void coin(int last_mov){
  cmPR(15+last_mov);
  delay(1000);
  cmReverse(12);
  delay(1000);
  brake();
  delay(500);
  cmForward(12);
  delay(1000);
  cmPL(15);
  delay(1000);
}

void loop() {
  digitalWrite(pinLED,LOW);

  if(state == 0) {// get to coins
  follow();
  Serial.println(d_count);
  digitalWrite(anim[0], HIGH);
    if(d_count>26000 && (measure_distance()<25) /*&& (turn > 90)*/){
      state=1;
      brake();
      digitalWrite(anim[1], HIGH);
      //Serial.println(turn);
      delay(1000);
      stsw=millis();
      coin(last_mov);
      follow();
      delay(100);
        follow();
        delay(50);
        follow();
        delay(50);
        follow();
        delay(50);
        follow();
        delay(50);
        
        
      coin(last_mov);
      follow();
      delay(50);
      brake();
      delay(200);
      cmForward(5);
      delay(1000);
      speed(3);
      while(measure_distance() > 10){
        follow();
        delay(100);
      }
      brake();
      coin(0);
      // cmForward(5);
      // delay(1000);
      // coin(0);
      //cmForward(10);
      delay(1000);
      cmReverse(10.5);
      delay(1000);
      cmPR(14.5);
      delay(1000);
      cmForward(22);
      delay(700);
      stsw = millis();
    }

  } else if(state == 1){
    follow();
    
    if(millis()-stsw > 3500){
      state = 2;
      digitalWrite(anim[2], HIGH);
      brake();
      cmPL(1.5);
      delay(500);
      // etime = millis();
      
      d_count=0;
      while(d_count < 4200) {
        reverse();
        delay(100);
      }
      brake();
    }
  } 
  
  else if (state == 2) { // initial color check
    speed(6);
    cmPR(16);
    delay(600);
    cmForward(13);
    delay(500);
    w_count = d_count;
    stsw = millis();
    while(d_count - w_count < 44*tpc && millis() - stsw < 2000) {
      follow();
    };
    coast();
    delay(500);
    cmPR(1.3);
    delay(500);
    state = 3;
    // Serial.println("ready for color");
  } else if (state == 3) {
    int color = color_loop();
    // int colc = color_loc[color];
    // if (colc == prev_loc) {
    //   hit();
    //   delay(500);
    //   color = color_loop();
    // }
    cmReverse(48);
    delay(1800);
    to_color(color);
    cmForward(10);
    delay(550);
    w_count = d_count;
    stsw = millis();
    while(d_count - w_count < 50*tpc && millis() - stsw < 2000) {
      follow();
    }
    coast();
    cmReverse(1.5);
    delay(400);
    hit();
    cmPR(1.8);
    delay(700);
    // if (etime - millis() > 60000*2)
    //   state = 4;
  }
}

int prev_loc = 0;
void to_color(int color) {
  color = col_loc[color];
  for(int i = 0; i<8; i++){ digitalWrite(anim[i], LOW); }
  digitalWrite(anim[color], HIGH);
  //while(buttonp == button) {}; // wait for press
  //buttonp = button;
  float diff = prev_loc - color;
  // if (abs(diff) > 2) diff *= 1.16;
  // diff *= 1 + diff / 30;
  if(diff < 0) {
    cmPL(4.6*abs(diff));
  } else {
    diff *= 1.1;
    cmPR(4.9*abs(diff));
  }

  delay(300*abs(diff));
  prev_loc = color;
}


void forward() {
 enA = true;
 in1 = true;
 in2 = false;

 enB = true;
 in1B = true;
 in2B = false;
 set_pins();
}


void reverse(){
 enA = true;
 in1 = false;
 in2 = true;

 enB = true;
 in1B = false;
 in2B = true;
 set_pins();
}


void brake(){
 enA = true;
 in1 = false;
 in2 = false;

 enB = true;
 in1B = false;
 in2B = false;
 set_pins();
}


void coast(){
 enA = false;
 in1 = false;
 in2 = false;

 enB = false;
 in1B = false;
 in2B = false;
 set_pins();
}


void turnLeft() {
 enA = false;

 enB = true;
 in1B = true;
 in2B = false;
 set_pins();
}
void turnRight() {
 enA = true;
 in1 = true;
 in2 = false;

 enB = false;
 set_pins();
}
void pivotLeft() {
 enA = true;
 in1 = false;
 in2 = true;

 enB = true;
 in1B = true;
 in2B = false;
 set_pins();
}
void pivotRight() {
 enA = true;
 in1 = true;
 in2 = false;

 enB = true;
 in1B = false;
 in2B = true;
 set_pins();
}

void measure() {
  d_count++;
  //Serial.println(count);
  if (move) {count++;
  //Serial.println(count);
  }
  if (move && count > target) {
    // Serial.println()
    move = false;
    brake();
  }
}

void cmForward(float x){
  count = 0;
  target = x*90 - 100 ; // -100 for braking time
  move = true;
  forward();
}

void cmReverse(float x) {
 count = 0;
 target = x*90 - 100 ; // -100 for braking time
 move = true;
 reverse();
}

void cmPR(float x) {
 count = 0;
 target = x*90 - 100 ; // -100 for braking time
 move = true;
 pivotRight();
}
void cmPL(float x) {
 count = 0;
 target = x*90 - 100 ; // -100 for braking time
 move = true;
 pivotLeft();
}
void cmTR(float x) {
 count = 0;
 target = x*90 - 100 ; // -100 for braking time
 move = true;
 turnRight();
}


void speed(int s) {
  signal_length = s*2;
}

void signal() {
  signal_counter++;
  if (signal_counter == 20) {
    digitalWrite(13, enA);
    digitalWrite(8, enB);
    signal_counter = 0;
  }
  if (signal_counter >= signal_length) {
    digitalWrite(8, LOW);
    digitalWrite(13, LOW);
  }
}

void set_pins() {
  digitalWrite(13, LOW);
  digitalWrite(8, LOW);
  digitalWrite(12, in1);
  digitalWrite(11, in2);
  digitalWrite(9, in1B);
  digitalWrite(10, in2B);
  digitalWrite(13, enA);
  digitalWrite(8, enB);
}
