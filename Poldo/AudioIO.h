#include"src/Audio.h"

#include "SPIFFS.h"
#include "FS.h"
#define wavHeader 44

class AudioIO{
  public:
    AudioIO();
    void setPinOut(int bclk, int lrc, int dout);
    void setMicrophone(adc1_channel_t adc);
    void setVolume(int vol);
    void Record(int seconds, int frequency= 16000);
    void Play(const char *filename);
    void ReproduceRecord();
  private:
  	void formatHeader(byte* header, int wavSize, int channels, int sample_rate, int BitsPerSample);
  	bool isPlaying();
    Audio out;
    //FS filesystem;
    adc1_channel_t mic_pin;
    byte header[wavHeader];
};
