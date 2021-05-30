/*
	Pappagallo
*/
//#include<Audio.h>

#include"Pappagallo.h"
#include"Arduino.h"
#include"EEPROM.h"

Pappagallo::Pappagallo(int head_pin, int ali_pin){
	head.attach(head_pin);
	ali.attach(ali_pin);
}
void Pappagallo::scanArea(){
  if (EEPROM.read(angles)==13){//I read address, if it contain value 13 it let me know I ve scan area data on the EEPROM
    for (int i= 0; i<angles; i++){
      scan_area[i]= EEPROM.read(i);
    }
  }else{//I haven't data I need to get it on other ways
    delta_angle= abs(delta_angle);
    int i;
    for (head_angle= minAngle; head_angle<maxAngle; head_angle+= delta_angle){
      turn_head();
      i= 0;
      int dist;
      do {
        dist= getDistance();//get object distance
        i++;
      }while((dist==0)&&(i<10));//while it's out of range and i<10 stay in loop
      scan_area[head_angle/delta_angle]= dist;
    }
    for (i= 0; i<angles; i++){
      EEPROM.write(i, scan_area[i]);//Write objects distance on EEPROM
    }
    EEPROM.write(angles, 13);//Set reference value
    EEPROM.commit();
  }
}
int Pappagallo::countAngles(){
  return angles;
}
void Pappagallo::setReproducer(int bclk, int lrc, int dout, int volume){
  audio.setPinOut(bclk, lrc, dout);
  audio.setVolume(volume);
}
void Pappagallo::setMic(adc1_channel_t microphone){
  audio.setMicrophone(microphone);
}
void Pappagallo::ear(){
  audio.fillSamples(buff, NUMSAMPLES);
}
bool Pappagallo::anyone_talk(){
  return human_talk;
}
void Pappagallo::wait_noise(byte noise, int timeout){
  int i, j;
  
  for (i= 0; i<timeout; i++){
    ear();//I fill buff array with samples of recording
    for (j= 6; i<numSamples; i++){ //I approximate the noise control as if it were instantaneous because the CPU works from a frequency of 80MHz up to 240MHz and in this case numSamples is relatively small
      if (((abs(buff[i]-buff[i-3])>noise)&&(abs(buff[i-3]-buff[i-6])>noise))){
        human_talk= true;
        Serial.println("sentito");
        return;
      }
    }
  }
}
bool Pappagallo::inRange(int a, int b, int delta){
  int value= abs(a-b);
  if (value>=delta)
    return true;
  return false;
}
int Pappagallo::check_humans_near(){
    int dist= 0;
    int counter= 0;

    do {
      for (int i= 0; ((i<10)&&(dist==0)); i++){
          dist= getDistance();
          int scan= scan_area[head_angle/abs(delta_angle)];
          if ((scan!=0)&&(inRange(dist, scan, 8))){//I check if exist a scan, if dist and scan are little different I would get out of range
            dist= 0;
            break;//I got an object, and I want to identify object as a out of range
          }
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
void Pappagallo::setDistanceSensor(int Trigger, int Echo){
  trigger= Trigger;
  echo= Echo;
}
void Pappagallo::Record(int seconds){
  if (human_talk){//Special record with a buff
    audio.RecordBuff(seconds, buff, NUMSAMPLES, true);
    human_talk= false;
  }else
	  audio.Record(seconds);//Regular record
}
void Pappagallo::Reproduce(const char *file){
	audio.Play(file);
}
void Pappagallo::Ripeti(){
  audio.ReproduceRecord();
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
    Serial.println(distanza);
    return distanza;
}
