#include <Wire.h>
#include "Adafruit_TCS34725.h"

#define MOTOR_A1  0
#define MOTOR_A2  0
#define MOTOR_B1  0
#define MOTOR_B2  0
#define MAX_SPEED 0

#define DARKNESS_UMBRAL 30
#define COLOR_DIF 30

#define BLACK   0
#define WHITE   1
#define RED     2
#define GREEN   3
#define BLUE    4
#define MAGENTA 5
#define YELLOW  6
#define CYAN    7

#define ledR    3
#define ledG    5
#define ledB    6

float red,green,blue,lux;
uint8_t color;
uint16_t colorTemp;

float redCald = 1, greenCald = 1, blueCald = 1;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

uint8_t getColor(float r, float g, float b, float l);
void calibrate(float *Rcal, float *Gcal, float *Bcal);

void setup() {
  
  Serial.begin(9600); // Serial communication for debugging
  
  Serial.println("Searching for sensor");
  while(!tcs.begin());  // Waits until sensor available
  Serial.println("Found sensor");
  Serial.println("Calibrating...");
  calibrate(&redCald, &greenCald, &blueCald); // Calibrates values
  Serial.println("Calibration completed");

  Serial.print("red: "); Serial.println(redCald);
  Serial.print("green: "); Serial.println(greenCald);
  Serial.print("blue: "); Serial.println(blueCald);

}


void loop() {
  
  //tcs.setInterrupt(false);
  
  // Gets data from sensor and process it by proportional calibration
  tcs.getRGB(&red, &green, &blue);
  red *= redCald;
  green *= greenCald;
  blue *= blueCald;
  
  lux = tcs.calculateLux(red,green,blue);
  colorTemp = tcs.calculateColorTemperature(uint16_t(red),uint16_t(green),uint16_t(blue));
  //tcs.setInterrupt(true);

  // Show data for debugging
  //Serial.print("TempColor:\t"); Serial.print(int(colorTemp));
  Serial.print("Lux:\t"); Serial.print(int(lux));
  Serial.print("\tR:\t"); Serial.print(int(red));
  Serial.print("\tG:\t"); Serial.print(int(green));
  Serial.print("\tB:\t"); Serial.println(int(blue));
  
  color = getColor(red, green, blue, lux); // Determines what color is being seeing
  
  switch(color)  {
    case BLACK:
      analogWrite(ledR, 255);
      analogWrite(ledG, 255);
      analogWrite(ledB, 255);
    break;
    case WHITE:
      analogWrite(ledR, 0);
      analogWrite(ledG, 0);
      analogWrite(ledB, 0);
    break;
    case RED:
      analogWrite(ledR, 255);
      analogWrite(ledG, 0);
      analogWrite(ledB, 0);
    break;
    case GREEN:
      analogWrite(ledR, 0);
      analogWrite(ledG, 255);
      analogWrite(ledB, 0);
    break;
    case BLUE:
      analogWrite(ledR, 0);
      analogWrite(ledG, 0);
      analogWrite(ledB, 255);
    break;
    case MAGENTA:
      analogWrite(ledR, 255);
      analogWrite(ledG, 0);
      analogWrite(ledB, 255);
    break;
    case YELLOW:
      analogWrite(ledR, 255);
      analogWrite(ledG, 255);
      analogWrite(ledB, 0);
    break;
    case CYAN:
      analogWrite(ledR, 0);
      analogWrite(ledG, 255);
      analogWrite(ledB, 255);
    break;
  }
}

uint8_t getColor(float r, float g, float b, float l){
    
  float rgDif, rbDif, gbDif;

  if(r > g) rgDif = r - g;
  else rgDif = g - r;

  if(r > b) rbDif = r - b;
  else rbDif = b - r;

  if(g > b) gbDif = g - b;
  else gbDif = b - g;

  if(r > g){ // More red than green
      
    if(r > b){ // And MORE than blue

      if(rgDif > COLOR_DIF && rbDif > COLOR_DIF) return RED; // Only red outstands
      else if(rgDif > COLOR_DIF && rbDif <= COLOR_DIF) return MAGENTA;  // red and blue outstands over green
      else if(rgDif <= COLOR_DIF && rbDif > COLOR_DIF) return YELLOW; // No color outstands
      else if(l >= DARKNESS_UMBRAL) return WHITE;
      else return BLACK;

    } else { // And LESS than blue
        
      if(rbDif > COLOR_DIF) return BLUE; // blue outstands
      else if(rgDif > COLOR_DIF) return MAGENTA; // red and blue outstands over green
      else if(l >= DARKNESS_UMBRAL) return WHITE; // No color outstands
      else return BLACK;
    }

  } else { // More green than red
      
    if(g > b){ // And MORE than blue

      if(rgDif > COLOR_DIF && gbDif > COLOR_DIF) return GREEN; // Only Green outstands
      else if(rgDif > COLOR_DIF && gbDif <= COLOR_DIF) return CYAN; // Green and Blue outstands over red
      else if(rgDif <= COLOR_DIF && gbDif > COLOR_DIF) return YELLOW; // Green and Red outstands over blue
      else if(l >= DARKNESS_UMBRAL) return WHITE; // No color outstands
      else return BLACK;

    } else {// And LESS than blue
      
      if(gbDif > COLOR_DIF) return BLUE; // Only Blue outstands
      else if(rgDif > COLOR_DIF) return CYAN; // Green and Blue outstands over Red
      else if(l >= DARKNESS_UMBRAL) return WHITE; // No color outstands
      else return BLACK;
    }
  }
}


void calibrate(float *Rcal, float *Gcal, float *Bcal){
  float Rred, Rgreen, Rblue;
  float Mred = 0, Mgreen = 0, Mblue = 0;
  *Rcal = 1;
  *Gcal = 1;
  *Bcal = 1;

  // 10 Samples of enviroment color
  for (uint8_t i = 0; i < 100; i++){
    tcs.getRGB(&Rred, &Rgreen, &Rblue);
    Mred += Rred;
    Mgreen += Rgreen;
    Mblue += Rblue;
  }
  // Mean values
  Mred /= 100;
  Mgreen /= 100;
  Mblue /= 100;

  // Calibration 
  
  if (Mred < Mgreen && Mred < Mblue){
    while (Mgreen * *Gcal - Mred * *Rcal > 0.1){
      *Gcal -= 0.001;
    }
    while (Mblue * *Bcal - Mred * *Rcal > 0.1){
      *Bcal -= 0.001;
    }
  }
  else if (Mgreen < Mred && Mgreen < Mblue){
    while (Mred * *Rcal - Mgreen * *Gcal > 0.1){
      *Rcal -= 0.001;
    }
    while ((Mblue * *Bcal - Mgreen * *Gcal) > 0.1){
      *Bcal -= 0.001;
    }
  }
  else if(Mblue < Mred && Mblue < Mgreen){
    while (Mred * *Rcal - Mblue * *Bcal > 0.1){
      *Rcal -= 0.001;
    }
    while (Mgreen * *Gcal - Mblue * *Bcal > 0.1){
      *Gcal -= 0.001;
    }    
  }
}
