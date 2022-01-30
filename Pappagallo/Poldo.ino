#include <ESP32_Servo.h>
//#include "Servo.h"

#include"Pappagallo.h"

#define FILESYSTEM SPIFFS

#define ALI 4
#define HEAD 2

#define LED 15
#define TRIG 5
#define PIR 19
#define ECHO 13
#define RESET 4

#define I2S_DOUT 25
#define I2S_BCLK 27 // I2S
#define I2S_LRC 26

#define VOLUME 10

Pappagallo poldo(HEAD, ALI, "Poldo");

volatile int motion= 10;

int timer, timer_vieni;

void getMotion(){
  motion= 10;
}

void setup() {
  FILESYSTEM.begin(true);
  
  Serial.begin(115200);
    
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PIR, INPUT);
  pinMode(RESET, INPUT);
  
  poldo.setDistanceSensor(TRIG, ECHO);
  poldo.setReproducer(I2S_BCLK, I2S_LRC, I2S_DOUT, VOLUME);
  poldo.setMic(ADC1_CHANNEL_5);
  poldo.setLight(LED);
  poldo.Dormi();
  //poldo.scanArea(); //I scan area to know where are objects, buggy function
  
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
      poldo.Dormi();
      if (secondi-timer>=10){
        timer= secondi;
        poldo.Batti_ali();
        poldo.Reproduce("Verso.wav");
        //poldo.Batti_ali(); with DC motor uncomment
        motion-= 2;//Diminuisco attività nella stanza
      } 
    }
    else{//Altrimenti controllo a che distanza sta l'umano
      motion= 10;
      timer_normalize= millis();
      poldo.Sveglio();
      
      if (human_distance<70){//Se è vicino almeno 70cm gli chiedo di parlarmi e io ascolto
        poldo.Batti_ali();
        poldo.Reproduce("Talk_me.wav");
        //poldo.Batti_ali(); with DC motor uncomment
        detachInterrupt(digitalPinToInterrupt(PIR));//disabilito interrupt per evitare conflitti nella record
        poldo.wait_noise(9, 3);//Aspetto che qualcuno mi parli, impostando un timeout
        if (poldo.anyone_talk()){ //La variabile human:talk diventa vera se wait noise ha sentito qualcosa nel campionamento
          poldo.Record(5); //Ascolto per 5 secondi chi mi sta parlando aggiungendo anche il campione precedente

          for (int i= 0; i<3; i++){
            poldo.Batti_ali();
            poldo.Ripeti();//Ripeto ciò che mi è stato detto
            //poldo.Batti_ali(); with DC motor uncomment
          }
        }
        attachInterrupt(digitalPinToInterrupt(PIR), getMotion, RISING);
      }
      else{
        if (secondi-timer_vieni>5){
          poldo.Batti_ali();
          poldo.Reproduce("Come_here.wav");
          //poldo.Batti_ali(); with DC motor uncomment
          timer_vieni= secondi;
        }
      }
      timer= secondi;
    }
  }
  /*
  if (digitalRead(RESET)){It'll be used in next update
    //poldo.resetScan(); useless
  }
  */
  int delta= 1000-((millis()-timer_normalize)%1000);//milliseconds to normalize time
  delay(delta);//Normalize time, loop recycle every second
}
