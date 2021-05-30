#include <ESP32_Servo.h>
//#include "Servo.h"

#include"Pappagallo.h"
#include"EEPROM.h"
//#include"SPIFFS.h"
//#include"src/Audio.h"
//#include "Arduino.h"

#define FILESYSTEM SPIFFS

#define ALI 4
#define HEAD 2

#define PIR 14
#define TRIG 5
#define ECHO 13
#define RESET 4

#define I2S_DOUT 25
#define I2S_BCLK 27 // I2S
#define I2S_LRC 26

Pappagallo poldo(HEAD, ALI);

volatile int motion= 10;

int timer, timer_vieni;

void getMotion(){
  motion= 10;
 // Serial.println("motioned");
}

void setup() {
  FILESYSTEM.begin(true);
  Serial.begin(115200);
  Serial.println("Ciao mi chiamo Poldo");
    
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PIR, INPUT);
  pinMode(RESET, INPUT);
  poldo.setDistanceSensor(TRIG, ECHO);
  poldo.setReproducer(I2S_BCLK, I2S_LRC, I2S_DOUT, 2);
  poldo.setMic(ADC1_CHANNEL_5);
  poldo.scanArea(); //I scan area to know where are 
  attachInterrupt(digitalPinToInterrupt(PIR), getMotion, RISING);

  timer_vieni= -5;
}

void loop() {
  unsigned long timer_normalize= millis();
  int secondi= (int)millis()/1000;;
  
  int human_distance; //Distanza pappagallo - uomo

  if (motion) { //Controllo se ci sono stati movimenti
    human_distance= poldo.check_humans_near(); //Mi cerco intorno umani vicino a me
  
    if (human_distance==0){//Se non ho trovato nessuno mi disattivo
      if (secondi-timer>=10){
        timer= secondi;
        poldo.Reproduce("Verso.wav");
        motion-= 5;//Disattivo attività nella stanza
      } 
    }
    else{//Altrimenti controllo a che distanza sta l'umano
      motion= 10;
      timer_normalize= millis();
      timer= secondi;
      if (human_distance<70){//Se è vicino almeno 70cm gli chiedo di parlarmi e io ascolto
        poldo.Reproduce("Talk_me.wav");
        poldo.wait_noise(9, 3);//Aspetto che qualcuno mi parli, impostando un timeout
        Serial.println(poldo.anyone_talk());
        if (poldo.anyone_talk()){ //La variabile human:talk diventa vera se wait noise ha sentito qualcosa nel campionamento
          poldo.Record(5); //Ascolto per 5 secondi chi mi sta parlando aggiungendo anche il campione precedente
          for (int i= 0; i<3; i++)
            poldo.Ripeti();//Ripeto ciò che mi è stato detto
        }  
      }
      else{
        if (secondi-timer_vieni>5){
          poldo.Reproduce("Come_here.wav");
          timer_vieni= secondi;
        }
      }
    }
  }
  if (digitalRead(RESET)){
    EEPROM.write(poldo.countAngles(), 0);//Azzero la referenza dello scan (scan vuoto o non valido quindi)
    EEPROM.commit();
    ESP.restart();
  }
  int delta= 1000-((millis()-timer_normalize)%1000);//milliseconds to normalize time
  delay(delta);//Normalize time, loop recycle every second
}
