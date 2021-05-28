#include <ESP32_Servo.h>
//#include "Servo.h"

#include"Pappagallo.h"

//#include"SPIFFS.h"
//#include"src/Audio.h"
//#include "Arduino.h"

#define FILESYSTEM SPIFFS

#define ALI 4
#define HEAD 2

#define PIR 14
#define TRIG 5
#define ECHO 13

#define I2S_DOUT 25
#define I2S_BCLK 27 // I2S
#define I2S_LRC 26

Pappagallo poldo(HEAD, ALI);

volatile int motion= 10;

int timer, timer_vieni;

void getMotion(){
  motion= 10;
  Serial.println("motioned");
}

void setup() {
  FILESYSTEM.begin(true);
  Serial.begin(115200);
  Serial.println("Ciao mi chiamo Poldo");
    
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PIR, INPUT);
  poldo.setDistanceSensor(TRIG, ECHO);
  poldo.setReproducer(I2S_BCLK, I2S_LRC, I2S_DOUT, 5);
  poldo.setMic(ADC1_CHANNEL_5);
  attachInterrupt(digitalPinToInterrupt(PIR), getMotion, RISING);
  
  /*a.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  a.setVolume(15);
  a.connecttoFS(FILESYSTEM, "Talk_me.wav");*/
  //poldo.Reproduce("Talk_me.wav");
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
        //Serial.println(timer);
        //Serial.println("\nverso");
        poldo.Reproduce("Verso.wav");
        motion-= 5;//Disattivo attività nella stanza
      } 
    }
    else{//Altrimenti controllo a che distanza sta l'umano
      motion= 10;
      timer_normalize= millis();
      timer= secondi;
      if (human_distance<70){//Se è vicino almeno 70cm gli chiedo di parlarmi e io ascolto
        //Serial.println("\nParlami che ripeto");
        poldo.Reproduce("Talk_me.wav");
        poldo.wait_noise(9);
        while (Serial.available()==0);
        Serial.readString();
        Serial.println("Recording");
        poldo.Record(5);
        listSPIFFS();
        for (int i= 0; i<3; i++)
          poldo.Ripeti();
      }
      else{
        if (secondi-timer_vieni>5){
          poldo.Reproduce("Come_here.wav");
          timer_vieni= secondi;
        }
      }
    }
  }

  int delta= 1000-((millis()-timer_normalize)%1000);//milliseconds to normalize time
  delay(delta);//Normalize time, loop recycle every second
}


void listSPIFFS(void) {
  Serial.println(F("\r\nListing SPIFFS files:"));
  static const char line[] PROGMEM =  "=================================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}
