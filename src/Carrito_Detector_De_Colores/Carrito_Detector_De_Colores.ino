#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
TCS34725_INTEGRATIONTIME_50MS,
TCS34725_GAIN_4X);

// ===== MOTOR A =====
#define IN1 9
#define IN2 8
#define ENA 10

// ===== MOTOR B =====
#define IN3 4
#define IN4 5
#define ENB 6

#define SPEED 50

void motorsForward(){
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  analogWrite(ENA,SPEED);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(ENB,SPEED);
}

void motorsReverse(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  analogWrite(ENA,SPEED);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  analogWrite(ENB,SPEED);
}

void motorsStop(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  analogWrite(ENA,0);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  analogWrite(ENB,0);
}

void setup() {

Serial.begin(115200);

// Motor A
pinMode(IN1,OUTPUT);
pinMode(IN2,OUTPUT);
pinMode(ENA,OUTPUT);

// Motor B
pinMode(IN3,OUTPUT);
pinMode(IN4,OUTPUT);
pinMode(ENB,OUTPUT);

// Sensor
if(!tcs.begin()){
  Serial.println("Sensor no encontrado");
  while(1);
}

Serial.println("Sensor listo");
}

void loop() {

float red,green,blue;
tcs.getRGB(&red,&green,&blue);

int R = int(red);
int G = int(green);
int B = int(blue);

Serial.print("R: ");Serial.print(R);
Serial.print("  G: ");Serial.print(G);
Serial.print("  B: ");Serial.println(B);

// ROJO
if(R > G && R > B){
  Serial.println("Color: RED");
  motorsForward();
}

// AZUL
else if(B > R && B > G){
  Serial.println("Color: BLUE");
  motorsReverse();
}

// OTROS
else{
  Serial.println("Color: OTHER");
  motorsStop();
}

delay(200);
}