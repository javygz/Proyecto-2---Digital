#include <Servo.h>
Servo miServo; 

const int segA = 2;
const int segB = 3;
const int segC = 4;
const int segD = 5;
const int segE = 6;
const int segF = 7;
const int segG = 8;
const int botonModo = 9;
const int ledR = 11;
const int ledG = 12;
const int ledB = 13;
const int buzzer = 14;
const int servoPin = 15; 
int modo = 0; 



void setup() {

  // Pines display
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);

  // Botones
  pinMode(botonModo, INPUT);
  pinMode(botonAlarma, INPUT);

  // LED RGB
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);

  // Buzzer
  pinMode(buzzer, OUTPUT);

  // Servo
  miServo.attach(servoPin); 
  }



void loop() {

  bool estadoBoton = digitalRead(botonModo);

  if (estadoAnteriorBoton == HIGH && estadoBoton == LOW) {
    modo++;
    if (modo > 2) {
      modo = 0;
    }
    delay(200);
  }


}

void setup() {

}

void loop() {

}
