/*
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
*/
