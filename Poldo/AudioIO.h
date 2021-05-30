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
    void RecordBuff(int seconds, byte* buff, int buff_len, bool pos, int frequency= 16000); //Record with a buff, position true if u want add buff to beginning of wav file and false to add buffer to the end.
    void fillSamples(byte* buff, int buff_len);
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
