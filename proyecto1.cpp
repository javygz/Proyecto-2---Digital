/**********************************************
*Universidad del Valle de Guatemala
*Nombre: Rodrigo Fernando Recinos Lopez
*Fecha: 31/07/2026
*       08/08/2026 Cambios sobre variables para valores en display
*Curso: Electronica digital 2
*Proyecto1: sistema embebido
***********************************************/
//librerias
#include <Arduino.h>
#include <stdint.h>
#include <driver/ledc.h>
#include "config.h"

//variables definidas
#define alarma 3000
#define canalr 0
#define canalg 1
#define canalb 2
#define canalservo 3
#define freqPWM 50
#define resolutionPWM 16





//pines display
const int segA = 14;
const int segB = 12;
const int segC = 33;
const int segD = 25;
const int segE = 32;
const int segF = 27;
const int segG = 26;
const int punto = 13;
//pines transistores
const int transis1 = 4;
const int transis2 = 5;
const int transis3 = 18;
//pines led rgb
const int ledR = 22;
const int ledG = 21;
const int ledB = 19;
//pin boton de lectura
const int bt1 = 23;
//sensor
const int lm35 = 35;
//motor servo
const int servo = 15;


//declaramos feeds especiales de Adafruit IO
AdafruitIO_Feed *canaltemperatura= io.feed("temperatura");
AdafruitIO_Feed *canalcolor = io.feed("led");
AdafruitIO_Feed *canalservo1 = io.feed("servo");
//variables globales
int decimal = 0;
int unidad = 0;
int decena = 0;
float temperatura1 = 0;
bool nuevalectura = false;

//variable activa por interrupcion
volatile byte displayActual = 0;
volatile unsigned long ultimoTiempo = 0;
const unsigned long debounceDelay = 250;

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

hw_timer_t *Timer1 = NULL;
//funciones prototipo

void initPWM(void);
void initservo(void);
void configTimer(void);
void mostrar(int);
void servoPWM(uint16_t);
//interrupcion para evitar un rebote en boton para adafruit io
void IRAM_ATTR interr(){
  unsigned long tiempoActual = millis();
  if (tiempoActual - ultimoTiempo > debounceDelay) {
    nuevalectura = true;
    ultimoTiempo = tiempoActual;
  }

}
//interrupcion que permite generar un multiplexeo utilizando transistores como indicadores
void IRAM_ATTR multiISR()
{
    portENTER_CRITICAL_ISR(&timerMux);

    digitalWrite(transis1, LOW);
    digitalWrite(transis2, LOW);
    digitalWrite(transis3, LOW);

    

    switch(displayActual)
    {
        //display que muestra el decimal de la temperatura
        case 0:
            mostrar(decimal);
            digitalWrite(transis1,HIGH);
            digitalWrite(punto,LOW);

            break;
        //display que muestra la unidad de la temperatura enciendo el punto que inicio al decimal
        case 1:
            mostrar(unidad);
            digitalWrite(transis2,HIGH);
            digitalWrite(punto,HIGH);

            break;
        //display que muestra la decena de la temperatura
        case 2:
            mostrar(decena);
            digitalWrite(transis3,HIGH);
            digitalWrite(punto,LOW);

            break;
    }

    displayActual++;

    if(displayActual>2)
        displayActual=0;

    portEXIT_CRITICAL_ISR(&timerMux);
}

//obtenemos valores del sensor
void leertemp()
{
    int lectura = analogRead(lm35);

    float voltaje = lectura * 3.3 / 4095.0; //3.3 V equivalentes a 4095 valor de 12 bits de resolución

    temperatura1 = voltaje * 100.0; //10 mV por cada grado celsius

    Serial.print(" ADC: ");
    Serial.print(lectura);

    Serial.print(" | Voltaje: ");
    Serial.print(voltaje, 3);

    Serial.print(" V | Temperatura: ");
    Serial.print(temperatura1, 1);

    Serial.println(" C");

}
//utilizamos el pulso pwm para mover el servo 
void moverservo()
{
    String posicion;
    if(temperatura1 < 23)
    {
        //ancho de pulso 0 grados
        servoPWM(1000);      // Cerrado
        posicion = "cerrado";
    }
    else if(temperatura1 >=23 && temperatura1 <25)
    {
        //ancho de pulso 45 grados
        servoPWM(1250);      // Medio
        posicion = "medio";
    }

    else if(temperatura1 >=25 && temperatura1 <27)
    {
        //ancho de pulso 45 grados
        servoPWM(1250);      // Medio
        posicion = "medio";
    }

    else if (temperatura1 >=27)
    {
        //ancho de pulso 90 grados
        servoPWM(2000);      // Abierto
        posicion = "abierto";
    }
    canalservo1->save(posicion);
}
//utilizamos las lecturas del sensor para mostrar un color
void medirtemp(){
    String colorActual;
  if(temperatura1 < 23)
    {
        ledcWrite(canalr,0);
        ledcWrite(canalg,0); //azul
        ledcWrite(canalb,255);
        colorActual = "#0000FF";
    }

    else if(temperatura1 >=23 && temperatura1 <25)
    {
        ledcWrite(canalr,0);
        ledcWrite(canalg,255); //verde
        ledcWrite(canalb,0);
        colorActual = "#00FF00";
    }

    else if(temperatura1 >=25 && temperatura1 <27)
    {
        ledcWrite(canalr,255);
        ledcWrite(canalg,255); //amarillo
        ledcWrite(canalb,0);
        colorActual = "#FFFF00";
    }

    else 
    {
        ledcWrite(canalr,255);
        ledcWrite(canalg,0); //rojo
        ledcWrite(canalb,0);
        colorActual = "#FF0000";
    }
    canalcolor->save(colorActual);
}


//obtenemos valores para mostras en cada display
void cambiosdisplay()
{
    int valor = temperatura1 * 10;

    decena = (valor / 100)%10; //obtenemos la decena del valor total y guardamos el residuo dividiendo dentro de 10 

    unidad = (valor / 10) % 10;//obtenemos la unidad del valor total y guardamos el residuo dividiendo dentro de 10

    decimal = valor % 10;//obtenemos el decimal del valor total y guardamos el residuo 
}


void setup() {
   Serial.begin(115200);

   io.connect();

  

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  canaltemperatura->get();
  canalservo1->get();
  canalcolor->get();
    //funciones generadores de pulso y timer
    initPWM();
    initservo();
    configTimer();

    // Transistores
    pinMode(transis1, OUTPUT);
    pinMode(transis2, OUTPUT);
    pinMode(transis3, OUTPUT);

    digitalWrite(transis1, LOW);
    digitalWrite(transis2, LOW);
    digitalWrite(transis3, LOW);

    // Display
    pinMode(segA, OUTPUT);
    pinMode(segB, OUTPUT);
    pinMode(segC, OUTPUT);
    pinMode(segD, OUTPUT);
    pinMode(segE, OUTPUT);
    pinMode(segF, OUTPUT);
    pinMode(segG, OUTPUT);
    pinMode(punto, OUTPUT);

    digitalWrite(punto, LOW);

    // LM35
    pinMode(lm35, INPUT);

    // Primera lectura
    leertemp();
    cambiosdisplay();
    moverservo();
    medirtemp();
    attachInterrupt(digitalPinToInterrupt(bt1), interr, FALLING);
  
}
//encendemos display segun valores recibidos
void mostrar(int data){

  
  int segmentos[7] = {
    segA, segB, segC,
    segD, segE, segF, segG
  };

  
  int numeros[10][7] = {
    {1,1,1,1,1,1,0}, //0
    {0,1,1,0,0,0,0}, //1
    {1,1,0,1,1,0,1}, //2
    {1,1,1,1,0,0,1}, //3
    {0,1,1,0,0,1,1}, //4
    {1,0,1,1,0,1,1}, //5
    {1,0,1,1,1,1,1}, //6
    {1,1,1,0,0,0,0}, //7
    {1,1,1,1,1,1,1}, //8
    {1,1,1,1,0,1,1}  //9
    
  };

  for(int i = 0; i < 7; i++){

    digitalWrite(segmentos[i], numeros[data][i]);
  }
}


void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();


  if (nuevalectura){
    nuevalectura = false;
    leertemp();
    medirtemp();
    moverservo();
    cambiosdisplay();
    canaltemperatura->save(temperatura1);


  }
  
}

//generar pulso led rgb
void initPWM(void){
    ledcSetup(canalr,5000,8);
    ledcAttachPin(ledR,canalr);

    ledcSetup(canalg,5000,8);
    ledcAttachPin(ledG,canalg);

    ledcSetup(canalb,5000,8);
    ledcAttachPin(ledB,canalb);

    ledcWrite(canalr,0);
    ledcWrite(canalg,0);
    ledcWrite(canalb,0);
  
}
//generar pulso motor servo
//identificar servo
void initservo(void){
  ledcSetup(canalservo, freqPWM, resolutionPWM);
    ledcAttachPin(servo, canalservo);
}
//mandar pulso
void servoPWM(uint16_t ancho_us)
{
    uint32_t duty = (ancho_us * 65535UL) / 20000UL; //sin signo y entero largo
    ledcWrite(canalservo, duty);
    Serial.print("Duty: ");
    Serial.print(duty);
}
//timer displays
void configTimer(void)
{
    Timer1 = timerBegin(0,80, true);
    timerAttachInterrupt(Timer1, &multiISR, true);
    timerAlarmWrite(Timer1,alarma,true);
    timerAlarmEnable(Timer1);
}
