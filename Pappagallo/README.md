My parrot use my AudioIO library.
AudioIO content:
  I2S audio output (BCLK, LRC, DATA) and a simple ADC sampler (16kHz)
  Output audio can carry different formats, mp3 wav...
  Input audio make a .wav file, maybe header of this file is to adjust
Parrot content:
  Function to setup environment...
    Constructor request 2 pins, for ALI motor and HEAD motor
    setDistanceSensor(int Trigger, int Echo)
    setReproducer(int bclk, int lrc, int dout, int volume);
    setMic(adc1_channel_t microphone);
 Method to:
  Play Audio (void Reproduce(const char *filename))
  Listen (void Record()) Save record overwrite last record
  Repeat (void Riproduci()) Repeat last record
  check_humans_near(); //turn head and looking for humans near, front of parrot
  Battere ali, move wings
  
