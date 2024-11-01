#include <TimerOne.h>
#include <QTRSensors.h>

QTRSensors qtr;

int thresh;
int readl[8];
volatile int button = 0;
volatile int buttonp = 0;
volatile int target = 0;
volatile int count = 0;
volatile int turn = 0;
volatile int signal_length = 14;
volatile int signal_counter = 0;
volatile int last_mov = 0;


volatile bool enA = false;
volatile bool in1 = false;
volatile bool in2 = false;

volatile bool enB = false;
volatile bool in1B = false;
volatile bool in2B = false;

volatile bool move = false;
volatile long stsw;
int state = 2;
volatile int d_count = 0;
volatile bool calibrated = false;
int anim[] = {32,33,34,35,36,37,38,39};
const int tpc = 86; // ticks/cm for wheels


void setup() {
  Serial.begin(9600);

  pinMode(44, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(48, OUTPUT);
  pinMode(49, OUTPUT);
  pinMode(50, OUTPUT);
  pinMode(51, OUTPUT);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), measure, CHANGE);


  coast();
  Timer1.initialize(2000);
  Timer1.attachInterrupt(signal);

  servo_setup();
  distance_setup();

  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){30, 28, 26, 24, 31, 29, 27, 25}, 8);

  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), buttonpress, FALLING);
  for(int i = 0; i < 8; i++)
    pinMode(anim[i], OUTPUT);

  digitalWrite(anim[7], HIGH);

  volatile int wait;
  stsw = millis();
  while (button == buttonp);
  buttonp = button;
  Serial.println("calibrating");

  for (int i = 0; i < 150; i++) {
    qtr.calibrate();
    delay(20);
  }

  digitalWrite(anim[6], HIGH);

  qtr.readCalibrated(readl);
  Serial.println(" ");

  Serial.println("ready for read");
  hit();
}

int pos;
long senstime = 0;
const int middle = 4000;
int direction = 0;
long lastmeas = 0;
int directions[] = {0, 0, 0, 0, 0};
int directionsi = 0;
int w_count = 0;
int col_loc[] = {0, 6, 1, 4, 5, 0, 2};



void loop() {
  float dist = measure_distance();
  if(state == 0) { // get to coins & push
    follow();
    Serial.println(d_count);
    digitalWrite(anim[0], HIGH);
    if(d_count > 317 * tpc && dist < 30){
      brake();
      speed(5);
      digitalWrite(anim[1], HIGH);
      delay(1000);
      stsw = millis();
      coin(last_mov);
      cmForward(13);
      delay(1000);

      coin(0);
      cmForward(9);
      delay(1000);

      coin(0);
      cmReverse(13);
      delay(1000);
      
      cmPR(33);
      delay(1000);
      forward();
      digitalWrite(anim[2], HIGH);
      digitalWrite(anim[3], HIGH); 
      state = 1;
    }
  } else if (state == 1){
    follow();
    if(millis()-stsw > 1000){
      state=2;
      digitalWrite(anim[2], HIGH);
      brake();
      delay(1000);
      //cmForward(5);
    }
  // } else if (state == 2) {
  //   int color = color_loop();
  //   color = col_loc[color];
  //   for(int i = 0; i<8; i++){ digitalWrite(anim[i], LOW); }
  //   digitalWrite(anim[color], HIGH);
  // }
  } else if (state == 2) {
    speed(6);
    cmPR(15.5);
    delay(600);
    cmForward(8);
    delay(500);
    w_count = d_count;
    stsw = millis();
    while(d_count - w_count < 50*tpc && millis() - stsw < 1900) {
      follow();
    };
    coast();
    delay(200);
    state = 3;
    // Serial.println("ready for color");
  } else if (state == 3) {
    int color = color_loop();
    cmReverse(51);
    delay(1800);
    to_color(color);
    cmForward(8);
    delay(550);
    w_count = d_count;
    stsw = millis();
    while(d_count - w_count < 50*tpc && millis() - stsw < 1900) {
      follow();
    }
    coast();
    cmReverse(1.5);
    delay(500);
    hit();
    delay(1000);
  }
}

// char whac1[] = {'n','y','b','r','p','g','w'};
int prev_loc = 0;
void to_color(int color) {
  color = col_loc[color];
  for(int i = 0; i<8; i++){ digitalWrite(anim[i], LOW); }
  digitalWrite(anim[color], HIGH);
  //while(buttonp == button) {}; // wait for press
  //buttonp = button;
  int diff = prev_loc - color;
  if(diff < 0)
    cmPL(4.9*abs(diff));
  else
    cmPR(4.9*abs(diff));
  
  delay(1600);
  prev_loc = color;
}

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
    // Serial.println(direction);

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
    }
  }
}

void buttonpress() {
  button += 1;
}

void coin(int last_mov) {
  speed(6);
  cmPR(12+last_mov);
  delay(1000);
  cmReverse(12);
  delay(1000);
  brake();
  delay(1000);
  cmForward(12);
  delay(1000);
  cmPL(56);
  delay(1000);
  speed(8);
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
  if (move) {
    count++;
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
  target = x*tpc - tpc/1.5; // -4 for braking time
  move = true;
  forward();
}

void cmReverse(float x) {
 count = 0;
 target = x*tpc - tpc/1.5;
 move = true;
 reverse();
}

void cmPR(float x) {
 count = 0;
 target = x*tpc - tpc/1.5;
 move = true;
 pivotRight();
}
void cmPL(float x) {
 count = 0;
 target = x*tpc - tpc/1.5;
 move = true;
 pivotLeft();
}
void cmTR(float x) {
 count = 0;
 target = x*tpc - tpc/1.5;
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
  if (signal_counter == signal_length) {
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
