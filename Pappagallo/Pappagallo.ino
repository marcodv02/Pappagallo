#include <ESP32_Servo.h>
//#include "Servo.h"
#include "src/Pappagallo.cpp"

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

void clear_files();
int COUNT_FILES();
void check_audio();

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

void check_audio(){
  int n_files= COUNT_FILES(); /* verso come_here talk_me*/
  if (n_files!=3)
    clear_files();
}
}
int COUNT_FILES(void){
  int num_files= 0;
  fs::File root= SPIFFS.open("/");

  fs::File file= root.openNextFile();
  do {
    file = root.openNextFile();
    
    if ((!file.isDirectory())&&(file)) {
      num_files++;
      
  }while (file);
  
  return num_files;
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
