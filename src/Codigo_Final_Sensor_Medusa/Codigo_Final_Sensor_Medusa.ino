#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
TCS34725_INTEGRATIONTIME_50MS,
TCS34725_GAIN_4X
);

// ===== MOTORES =====
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// ===== BUZZER =====
#define BUZZER 7

// ===== MOVIMIENTOS =====

void motorsForward(){
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void Tback(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void motorsStop(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}

void turnLeft(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
}

void turnRight(){
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
}

void beep(){
  digitalWrite(BUZZER,HIGH);
  delay(300);
  digitalWrite(BUZZER,LOW);
}

void setup(){

Serial.begin(115200);

pinMode(IN1,OUTPUT);
pinMode(IN2,OUTPUT);
pinMode(IN3,OUTPUT);
pinMode(IN4,OUTPUT);
pinMode(BUZZER,OUTPUT);

if(!tcs.begin()){
  Serial.println("Sensor no encontrado");
  while(1);
}

Serial.println("Sensor listo");

}

void loop(){

float red,green,blue;
tcs.getRGB(&red,&green,&blue);

int R=int(red);
int G=int(green);
int B=int(blue);

Serial.print("R: ");Serial.print(R);
Serial.print(" G: ");Serial.print(G);
Serial.print(" B: ");Serial.println(B);


// ===== AMARILLO =====
if(R > 100 && G > 90 && B < 50){
  Serial.println("Color: AMARILLO -> BUZZER");
  motorsStop();
  beep();
}

// ===== MAGENTA =====
else if(R > 108 && B > 100 && G > 70 && R > B){
  Serial.println("Color: MAGENTA -> GIRANDO IZQUIERDA");
  turnLeft();
}

// ===== MORADO =====
else if(B > 100 && R > 100 && G < 90 && B > R){
  Serial.println("Color: MORADO -> GIRANDO DERECHA");
  turnRight();
}

// ===== ROJO =====
else if(R > 120 && R > G && R > B){
  Serial.println("Color: ROJO -> DETENIDO");
  motorsStop();
}

// ===== VERDE =====
else if(G > 120 && G > R && G > B){
  Serial.println("Color: VERDE -> AVANZANDO");
  motorsForward();
}

else if(B > 100 && B > G && B > R && R < 70){
  Serial.println("Color: Azul -> Girando hacia atras");
  Tback();
}

else{
  Serial.println("Color: OTRO");
  motorsStop();
}

delay(200);

}