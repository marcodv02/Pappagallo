/*
	Header file of Pappagallo
*/
//#include<Audio.h>
#include"AudioIO.h"
#include <ESP32_Servo.h>

class Pappagallo{
	protected:
    bool motion_detect= false;
    int trigger, echo;
    Servo head, ali;
    int max_distance= 200;
    int delta_angle= 20;
    const int MIN_ANGLE= 20;
    const int MAX_ANGLE= 160;
    int head_angle= 60;
	public:
		Pappagallo(int head_pin, int ali_pin);
		int check_humans_near();
    void setDistanceSensor(int Trigger, int Echo);
    void setReproducer(int bclk, int lrc, int dout, int volume);
    void setMic(adc1_channel_t microphone);
		void wait_noise(int noise);
		void Batti_ali();
		void Record(int seconds);
		void Reproduce(const char *file);
    void Ripeti();
		void Dormi();
		void Sveglio();
	private:
		void turn_head();
		int Get_distance();
		int ear();
    AudioIO audio;
};
