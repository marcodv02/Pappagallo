/*
	Pappagallo
*/
//#include<Audio.h>

#include"Pappagallo.h"
#include"Arduino.h"

Pappagallo::Pappagallo(int head_pin, int ali_pin){
	head.attach(head_pin);
	ali.attach(ali_pin);
}

void Pappagallo::setReproducer(int bclk, int lrc, int dout, int volume){
  audio.setPinOut(bclk, lrc, dout);
  audio.setVolume(volume);
}
void Pappagallo::setMic(adc1_channel_t microphone){
  audio.setMicrophone(microphone);
}
int Pappagallo::check_humans_near(){
    int dist= 0;
    int counter= 0;

    do {
      for (int i= 0; ((i<10)&&(dist==0)); i++){
          dist= Get_distance();
          Serial.println(dist);
      }
      if (dist==0){
        head_angle+= delta_angle;
        turn_head();
        
        if ((head_angle==MAX_ANGLE)||(head_angle==MIN_ANGLE)){
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
	audio.Record(seconds);
}
void Pappagallo::Reproduce(const char *file){
	audio.Play(file);
}
void Pappagallo::Ripeti(){
  Serial.println("ripeto");
  audio.ReproduceRecord();
}

void Pappagallo::turn_head(){
  head_angle= constrain(head_angle, MIN_ANGLE, MAX_ANGLE);
  head.write(head_angle);
	delay(10);
}

int Pappagallo::Get_distance(){
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    unsigned long durata= pulseIn(echo, HIGH, 10000);
    int distanza= (int)durata/60;//It should be 58, but I like aproximation  and 60 is more mnemonic
    distanza= (distanza>max_distance) ? 0 : distanza;//If it's out of range I get 0
    Serial.println(distanza);
    return distanza;
}
void Pappagallo::wait_noise(int noise){
  
}
