#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <stdbool.h>
#include <stdlib.h>

/* Example code for the Adafruit TCS34725 breakout library */


/* Connect SCL    to analog 5
  Connect SDA    to analog 4
  Connect VDD    to 3.3V DC
  Connect GROUND to common ground */


/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

float vals[] = {0.51, 0.33, 0.16,     0.05, 0.24, 0.71,     0.69, 0.16, 0.15,     0.18, 0.21, 0.62 ,     0.14, 0.64, 0.22,     0.22, 0.35, 0.43};
float vals1[] = {'y','b','r','p','g','w'};
char whac1[] = {'n','y','b','r','p','g','w'};

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
void setup(void) {
 Serial.begin(9600);




 if (tcs.begin()) {
   Serial.println("Found sensor");
 } else {
   Serial.println("No TCS34725 found ... check your connections");
   while (1);
 }


 // Now we're ready to get readings!
}
int sv;
int cnt=0;
uint16_t f;
bool p10(float x, float y){
  //Serial.println(y);
  float k = (y-x)/x;
  // Serial.print(k);
  // Serial.print(" ");
  if ( k <= 0.4 && k >= -0.4){
    //Serial.println(k);
    Serial.print("t ");
    return true;
  }
  else{
    Serial.print("f ");
    return false;
  }
}

void loop(void) {


// int x =  millis();
 uint16_t r, g, b, c, colorTemp, lux;
 float rc,gc,bc,cc;
 char p;
 float k[3];//rr,gr,br;


 tcs.getRawData(&r, &g, &b, &c);
 colorTemp = tcs.calculateColorTemperature(r, g, b);
 colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
 lux = tcs.calculateLux(r, g, b);


//  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
//  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
//  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
//  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
//  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
//  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
//  Serial.println(" ");


//  int y = millis();
//   float time = y-x;
//  Serial.print("The amount of time it takes the loop to run is: "); Serial.println(time);

//find ratios
rc=r;
gc=g;
bc=b;
//cc=c;//0.
//cc = colorTemp;
cc = rc + gc + bc;
k[0] = rc/cc;
k[1]=gc/cc;
k[2] = bc/cc;
Serial.print(k[0]);Serial.print(", ");Serial.print(k[1]);Serial.print(", ");Serial.print(k[2]);
Serial.println(" ");
cnt=0;
for(f=0;f<18;f++){
  
  if(f%3==0){

    Serial.print(" | ");Serial.print(f/3);Serial.print(" | ");
    }

  //Serial.print(cnt );
  if( p10(vals[f],k[f%3])){
    //Serial.print(cnt);
    //Serial.print(" ");
    cnt=cnt+1;
    //Serial.print(cnt);
    //Serial.print(" ");
    
  }
  //Serial.print(" {");Serial.print(f);Serial.print(" } ");
// Serial.println("here");
  if(cnt==3){
    sv=f;
    //Serial.println(f);
   //Serial.println("true");
   Serial.println(whac1[(f+cnt)/3]);
    break;
  }

  else if((f+1)%3==0){
    cnt=0;
  // Serial.print(" | ");Serial.print(f/3);Serial.print(" | ");
  
  }

}

// Serial.println(k[0]);
//Serial.println(sv);
//Serial.println(whac1[(sv+cnt)/3]);
delay(100);
}
