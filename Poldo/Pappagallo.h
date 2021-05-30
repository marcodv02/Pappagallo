/*
	Header file of Pappagallo
*/
//#include<Audio.h>
#include"AudioIO.h"
#include <ESP32_Servo.h>

//Buffer size for ear() function
#define NUMSAMPLES 16000*1 

#define MIN_ANGLE 20
#define MAX_ANGLE 160
#define DELTA_ANGLE 20
#define ANGLES (MAX_ANGLE-MIN_ANGLE)/DELTA_ANGLE

class Pappagallo{
  protected:
    const int minAngle= 20;
    const int maxAngle= 160;
    const int numSamples= NUMSAMPLES;
    const int angles= ANGLES;
    
    bool motion_detect= false;
    int trigger, echo;
    int max_distance= 200;
    int delta_angle= DELTA_ANGLE;
    int head_angle= 60;
    int scan_area[ANGLES];
    
    Servo head, ali;
    byte buff[NUMSAMPLES];
    bool human_talk= false;
    bool inRange(int a, int b, int delta);
	public:
		Pappagallo(int head_pin, int ali_pin);
    void setDistanceSensor(int Trigger, int Echo);
    void setReproducer(int bclk, int lrc, int dout, int volume);
    void setMic(adc1_channel_t microphone);
    void wait_noise(byte noise, int timeout);
		void Batti_ali();
		void Record(int seconds);
		void Reproduce(const char *file);
    void Ripeti();
    int countAngles();
    int check_humans_near();
    void scanArea();
    bool anyone_talk();
		void Dormi();
		void Sveglio();
	private:
		void turn_head();
		int getDistance();
		void ear();
    AudioIO audio;
};
