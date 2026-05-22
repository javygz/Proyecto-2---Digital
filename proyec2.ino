#include <Servo.h>
Servo Servo1, Servo2; 


const int segA = 2;
const int segB = 3;
const int segC = 4;
const int segD = 5;
const int segE = 6;
const int segF = 7;
const int segG = 8;
const int botonM = 9;
const int ledB = 10;
const int ledR = 11;
const int ledG = 12;
const int botonA = 14;
const int buzzer = 15;
const int servoPin1 = A0; 
const int servoPin2 = A4;
int modo = 0; 
int estadoAnteriorBoton = LOW;
int estadoAnteriorAlarma = LOW;
int lpm = 0;
int value1 = 0;
int grado = 0;
int potenciometro = A3;
int value2 = 0;
int temperatura = 0;
 
void ledRGB(int r,int g,int b){
  digitalWrite(ledR,r);
  digitalWrite(ledG,g);
  digitalWrite(ledB,b);
} 

void Modos(int modos){

  
  int segmentos[7] = {
    segA, segB, segC,
    segD, segE, segF, segG
  };

  
  int numeros[3][7] = {

    {1,1,1,1,1,1,0}, 
    {0,1,1,0,0,0,0}, 
    {1,1,0,1,1,0,1}  
  };

  for(int i = 0; i < 7; i++){

    digitalWrite(segmentos[i], numeros[modos][i]);
  }
}

void modozero(){
    ledRGB(LOW,LOW,LOW);

    noTone(buzzer);

    Servo1.write(0);
    Servo2.write(0);

    Modos(0);
    
    

}

void modouno(){
    value1 = analogRead(potenciometro);
    lpm = map(value1, 0,1023,0,200);
    grado = map(lpm,0,200,0,180);

    Modos(1);

    Servo1.write(grado);

    noTone(buzzer);

    if (lpm < 60){
      ledRGB(LOW,HIGH,LOW);
    }
    else if ((60 <= lpm) && (lpm <= 100)){
      ledRGB(LOW,LOW,HIGH);
    }
    else if ((150 >= lpm) && (lpm > 100)){
      ledRGB(HIGH,LOW,LOW);
    }
    else{
      ledRGB(HIGH,LOW,LOW);
      delay(200);
      ledRGB(LOW,LOW,LOW);
      delay(200);
      
    }
}

void mododos(){

    value2 = analogRead(potenciometro);
    temperatura = map(value2,0,1023,20,45);
    
    Modos(2);

    if (temperatura < 35) {
    ledRGB(HIGH,LOW,HIGH);

    Servo2.write(0);

    
    if (digitalRead(botonA) == HIGH) {
      tone(buzzer, 1000);
    }
    else {
      noTone(buzzer);
    }
  }

  
  else if ((temperatura >= 35) && (temperatura < 37)) {
    ledRGB(LOW,HIGH,HIGH);
    
    Servo2.write(45);

    noTone(buzzer);
  }

  else if ((temperatura >= 37) && (temperatura < 38)) {
    ledRGB(HIGH,HIGH,LOW);

    Servo2.write(90);

    noTone(buzzer);
  }

  
  else if ((temperatura >= 38) && (temperatura < 39)) {
    ledRGB(HIGH,HIGH,LOW);

    Servo2.write(135);

    noTone(buzzer);
  }

  
  else {
    ledRGB(HIGH,HIGH,HIGH);

    Servo2.write(180);
    
    if (digitalRead(botonA) == HIGH) {
      tone(buzzer, 1000);
    }
    else {
      noTone(buzzer);
    }
  }

}

void setup() {

  
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);


  pinMode(botonM, INPUT);
  pinMode(botonA, INPUT);

  
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);

  
  pinMode(buzzer, OUTPUT);

  
  Servo1.attach(servoPin1); 
  Servo2.attach(servoPin2);
  
  }



void loop() {

  int estadoBoton = digitalRead(botonM);
  int estadoalarma = digitalRead(botonA);
  delay(15);
  if (estadoBoton == HIGH){
    estadoAnteriorBoton = 1;
  }
  if (estadoAnteriorBoton == HIGH && estadoBoton == LOW) {
    modo++;
    if (modo == 3) {
      modo = 0;
    }
    estadoAnteriorBoton = 0;
    delay(15);
  }

  if (estadoalarma == HIGH){
    estadoAnteriorAlarma = 1;
  }
  if (estadoAnteriorAlarma == HIGH && estadoalarma == LOW) {
    estadoAnteriorAlarma = 0;
    delay(15);
  }
  
  switch(modo){
    case 0:
    modozero();
    break;
    case 1:
    modouno();
    break;
    case 2:
    mododos();
    break;



  }
    

  

}


