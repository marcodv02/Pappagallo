#include <ESP32_Servo.h>
//#include "Servo.h"

#define ALI 4
#define HEAD 2

#define PIR 14
#define TRIG 12
#define ECHO 13

class Pappagallo{
  protected:
    bool motion_detect= false;
    Servo head, ali;
    int max_distance= 200;
    int delta_angle= 20;
    const int MIN_angle= 20;
    const int MAX_angle= 160;
  public:
  int head_angle= 60;
  Pappagallo(int head_pin, int ali_pin){
    head.attach(head_pin);
    ali.attach(ali_pin);
  }
  void Sveglio(){
    
  }
  void Dormi(){
    
  }
  void Reproduce(String file){
    Serial.print("Riproduco:\t");
    Serial.println(file);
  }
  void Record(int seconds){
    
  }
  void Batti_ali(){
    ali.write(0);
    delay(50);
    ali.write(180);
    delay(50);
  }
  int ear(){
    return 10;
  }
  void wait_noise(int noise){
    while (ear()<noise){
      
    }
  }
  int check_humans_near(int trigger, int echo){
    int dist= 0;
    int counter= 0;

    do {
      for (int i= 0; ((i<10)&&(dist==0)); i++){
          dist= Get_distance(TRIG, ECHO);
      }
      if (dist==0){
        head_angle+= delta_angle;
        turn_head(head_angle);
        
        if ((head_angle==MAX_angle)||(head_angle==MIN_angle)){
          delta_angle= -delta_angle;
          counter++;
        }
      }
    }while ((dist==0)&&(counter<2));
    return dist;
  }
  
  private:
  void turn_head(int angolo){
    angolo= constrain(angolo, 0, 180);
    head.write(angolo);
    delay(10);
  }
  int Get_distance(int trigger, int echo){
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    unsigned long durata= pulseIn(echo, HIGH, 10000);
    int distanza= (int)durata/60;
    distanza= (distanza>max_distance) ? 0 : distanza;

    return distanza;
  }
};

Pappagallo poldo(HEAD, ALI);

volatile int motion= 10;
unsigned long timer, timer2;
void ciao(){
  motion= 10;
  //motion= constrain(motion, 0, 10);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Ciao mi chiamo Poldo");
    
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PIR, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIR), ciao, RISING);

  timer2= millis();
  Serial.println(timer2);
}

void loop() {
  int human_distance; //Distanza pappagallo - uomo
  unsigned long timer2= millis();
  //Serial.println(motion);
  if (motion) { //Controllo se ci sono stati movimenti
    human_distance= poldo.check_humans_near(TRIG, ECHO); //Mi cerco intorno umani vicino a me
  
    if (human_distance==0){//Se non ho trovato nessuno mi disattivo
      if (millis()-timer>9000){
        timer= millis();
        Serial.println(timer);
        Serial.println("\nverso");
        poldo.Reproduce("Verso.mp3");
        motion--;//Disattivo attività nella stanza
      }
      
    }
    else{//Altrimenti controllo a che distanza sta l'umano
      motion= 10;
      timer2= millis();
      timer= timer2;
      if (human_distance<70){//Se è vicino almeno 70cm gli chiedo di parlarmi e io ascolto
        Serial.println("\nParlami che ripeto");
        poldo.Reproduce("Talk_me.mp3");
        poldo.wait_noise(9);
        poldo.Record(5);
        for (int i= 0; i<3; i++)
          poldo.Reproduce("Last.mp3");
      }
      else{
        poldo.Reproduce("Come_here.mp3");
        Serial.println("\nAvvicinati che parliamo");
      }
    }
  }
  //Serial.print("Time loop: ");
  int delta= 1000-((millis()-timer2)%1000);
  //int delta= constrain(900+timer2-millis(), 0, 1000);
  delay(delta);
  //Serial.println(millis());
}
