/*
	Pappagallo
*/
//#include<Audio.h>

#include"Pappagallo.h"
#include"Arduino.h"
//#include"src/analogWrite.h"
#include<EEPROM.h>

#define EEPROM_SIZE 256

Pappagallo::Pappagallo(int head_pin, int ali_pin, String nome){//Constructor
  //EEPROM.begin(EEPROM_SIZE);
  parrot_name= nome;
  Serial.print("Ciao mi chiamo ");
  Serial.println(nome);
  scanArea();//I fill scan with 0
	head.attach(head_pin);
  if (aliServo)//Set up ali as Servo
	  ali.attach(ali_pin);
  else//Set up ali as DC Motor
    ali_dc= ali_pin;
}
void Pappagallo::setDistanceSensor(int Trigger, int Echo){
  trigger= Trigger;
  echo= Echo;
}
void Pappagallo::setLight(int pin){
  led= pin;
}
void Pappagallo::setReproducer(int bclk, int lrc, int dout, int volume){//Set up I2S Reproducer
  audio.setPinOut(bclk, lrc, dout);
  audio.setVolume(volume);
}
void Pappagallo::setMic(adc1_channel_t microphone){//Set up audio input using ADC
  audio.setMicrophone(microphone);
}
void Pappagallo::ear(){//record a second and save it in buffer
  audio.fillSamples(buff, NUMSAMPLES);
}
void Pappagallo::wait_noise(byte noise, int timeout){//Function to wait human talk, with a timeout
  int i, j;
  unsigned long tt= millis();
  human_talk= false;
  
  for (i= 0; i<timeout; i++){
    ear();//I fill buff array with samples of recording
    for (j= 6; i<numSamples; i++){ //I approximate the noise control as if it were instantaneous because the CPU works from a frequency of 80MHz up to 240MHz and in this case numSamples is relatively small  
      if (((abs(buff[i]-buff[i-3])>noise)&&(abs(buff[i-3]-buff[i-6])>noise))){//A strange check for a strange noise will be improved with tests
        human_talk= true;
        return;
      }
    }
  }
}
void Pappagallo::scanArea(){//useless function for now, you need to wait upgrades
  for (int i= 0; i<angles; i++){
    scan_area[i]= 0;
  }
}
int Pappagallo::countAngles(){//return angles number
  return angles;
}
bool Pappagallo::anyone_talk(){
  return human_talk;
}
void Pappagallo::resetScan(){
  EEPROM.write(angles, 0);//Azzero la referenza dello scan (scan vuoto o non valido quindi)
  EEPROM.commit();
  ESP.restart();
}
bool Pappagallo::inRange(int a, int b, int delta){//Valore assoluto di sottrazione tra due numeri minore di delta? 
  int value= abs(a-b);
  if (value>=delta)
    return true;
  return false;
}
int Pappagallo::check_humans_near(){//mi controllo attorno
    int dist= 0;
    int counter= 0;

    do {
      for (int i= 0; ((i<10)&&(dist==0)); i++){
          dist= getDistance();
          int scan= scan_area[head_angle/abs(delta_angle)];
          /*
          if ((scan!=0)&&(inRange(dist, scan, 8))){//I check if exist a scan, if dist and scan are little different I would get out of range
            dist= 0;          Futute function
            break;//I got an object, and I want to identify object as a out of range
          }*/
      }
      if (dist==0){
        head_angle+= delta_angle;
        turn_head();
        
        if ((head_angle==maxAngle)||(head_angle==minAngle)){
          delta_angle= -delta_angle;
          counter++;
        }
      }
    }while ((dist==0)&&(counter<2));
    return dist;
}
void Pappagallo::Record(int seconds){
  if (human_talk){//Special record with implement of buff
    audio.RecordBuff(seconds, buff, NUMSAMPLES, true);
    human_talk= false;
  }else
	  audio.Record(seconds);//Regular record
}
void Pappagallo::Reproduce(const char *file){
	audio.Play(file);
}
void Pappagallo::Ripeti(){
  audio.ReproduceRecord();//Reproduce last record
}
void Pappagallo::Sveglio(){
  digitalWrite(led, HIGH);
}
void Pappagallo::Dormi(){
  digitalWrite(led, LOW);
}
void Pappagallo::Batti_ali(){
  int delta, i;
  bool on_off;
  if (aliServo){
    delta= abs(delta_angle);
    for (i= MIN_ANGLE; i<MAX_ANGLE; i+=delta){
      ali.write(i);
      delay(50);
    }
    for (i= MAX_ANGLE; i<MIN_ANGLE; i-=delta){
      ali.write(i);
      delay(50);
    }
  }else{
    on_off= digitalRead(ali_dc);
    digitalWrite(ali_dc, !on_off);
  }
}
void Pappagallo::turn_head(){
  head_angle= constrain(head_angle, minAngle, maxAngle);
  head.write(head_angle);
	delay(10);
}
int Pappagallo::getDistance(){
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    unsigned long durata= pulseIn(echo, HIGH, 10000);
    int distanza= (int)durata/60;//It should be 58, but I like aproximation  and 60 is more mnemonic
    distanza= (distanza>max_distance) ? 0 : distanza;//If it's out of range I get 0
    return distanza;
}
