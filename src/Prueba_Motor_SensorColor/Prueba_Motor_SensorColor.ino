#include <Wire.h>
#include "Adafruit_TCS34725.h"

// ===== Pines del L298N =====
#define MOTOR_A1 8
#define MOTOR_A2 9
#define ENA 10
#define MAX_SPEED 100

// ===== Configuración de detección =====
#define DARKNESS_UMBRAL 30
#define COLOR_DIF 30

// ===== Identificación de colores =====
#define BLACK   0
#define WHITE   1
#define RED     2
#define GREEN   3
#define BLUE    4
#define MAGENTA 5
#define YELLOW  6
#define CYAN    7

// ===== Pines LED RGB =====
#define ledR 3
#define ledG 5
#define ledB 6

float red,green,blue,lux;
uint8_t color;
uint16_t colorTemp;

float redCald = 1, greenCald = 1, blueCald = 1;

// Inicializa sensor
Adafruit_TCS34725 tcs =
Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// ===== Prototipos =====
uint8_t getColor(float r, float g, float b, float l);
void calibrate(float *Rcal, float *Gcal, float *Bcal);

void motorForward();
void motorReverse();
void motorStop();

void setup() {

  Serial.begin(9600);

  // Pines motor
  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Pines LED
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);

  Serial.println("Searching for sensor");

  while(!tcs.begin());

  Serial.println("Found sensor");
  Serial.println("Calibrating...");

  calibrate(&redCald, &greenCald, &blueCald);

  Serial.println("Calibration completed");

  Serial.print("red: "); Serial.println(redCald);
  Serial.print("green: "); Serial.println(greenCald);
  Serial.print("blue: "); Serial.println(blueCald);
}

void loop() {

  // Lectura del sensor
  tcs.getRGB(&red, &green, &blue);

  red *= redCald;
  green *= greenCald;
  blue *= blueCald;

  lux = tcs.calculateLux(red,green,blue);
  colorTemp = tcs.calculateColorTemperature(uint16_t(red),uint16_t(green),uint16_t(blue));

  // Debug
  Serial.print("Lux:\t"); Serial.print(int(lux));
  Serial.print("\tR:\t"); Serial.print(int(red));
  Serial.print("\tG:\t"); Serial.print(int(green));
  Serial.print("\tB:\t"); Serial.println(int(blue));

  color = getColor(red, green, blue, lux);

  switch(color){

    case RED:
      analogWrite(ledR,100);
      analogWrite(ledG,0);
      analogWrite(ledB,0);
      motorForward();
    break;

    case BLUE:
      analogWrite(ledR,0);
      analogWrite(ledG,0);
      analogWrite(ledB,70);
      motorReverse();
    break;

    default:
      analogWrite(ledR,0);
      analogWrite(ledG,0);
      analogWrite(ledB,0);
      motorStop();
    break;
  }

  delay(200);
}

// ===== Control del motor =====

void motorForward(){
  digitalWrite(MOTOR_A1,HIGH);
  digitalWrite(MOTOR_A2,LOW);
  analogWrite(ENA,MAX_SPEED);
}

void motorReverse(){
  digitalWrite(MOTOR_A1,LOW);
  digitalWrite(MOTOR_A2,HIGH);
  analogWrite(ENA,MAX_SPEED);
}

void motorStop(){
  digitalWrite(MOTOR_A1,LOW);
  digitalWrite(MOTOR_A2,LOW);
  analogWrite(ENA,0);
}

// ===== Detección de color =====

uint8_t getColor(float r, float g, float b, float l){

  float rgDif = abs(r-g);
  float rbDif = abs(r-b);
  float gbDif = abs(g-b);

  if(r>g && r>b && rgDif>COLOR_DIF && rbDif>COLOR_DIF)
    return RED;

  if(b>r && b>g && rbDif>COLOR_DIF && gbDif>COLOR_DIF)
    return BLUE;

  if(g>r && g>b && rgDif>COLOR_DIF && gbDif>COLOR_DIF)
    return GREEN;

  if(l >= DARKNESS_UMBRAL)
    return WHITE;

  return BLACK;
}

// ===== Calibración =====

void calibrate(float *Rcal, float *Gcal, float *Bcal){

  float Rred, Rgreen, Rblue;
  float Mred=0, Mgreen=0, Mblue=0;

  *Rcal=1;
  *Gcal=1;
  *Bcal=1;

  for (uint8_t i=0;i<100;i++){
    tcs.getRGB(&Rred,&Rgreen,&Rblue);
    Mred += Rred;
    Mgreen += Rgreen;
    Mblue += Rblue;
  }

  Mred/=100;
  Mgreen/=100;
  Mblue/=100;

  if (Mred < Mgreen && Mred < Mblue){

    while (Mgreen* *Gcal - Mred* *Rcal >0.1)
      *Gcal -=0.001;

    while (Mblue* *Bcal - Mred* *Rcal >0.1)
      *Bcal -=0.001;
  }

  else if (Mgreen < Mred && Mgreen < Mblue){

    while (Mred* *Rcal - Mgreen* *Gcal >0.1)
      *Rcal -=0.001;

    while (Mblue* *Bcal - Mgreen* *Gcal >0.1)
      *Bcal -=0.001;
  }

  else{

    while (Mred* *Rcal - Mblue* *Bcal >0.1)
      *Rcal -=0.001;

    while (Mgreen* *Gcal - Mblue* *Bcal >0.1)
      *Gcal -=0.001;
  }
}