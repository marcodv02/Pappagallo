#include"AudioIO.h"
#include"SPIFFS.h"

#define wavHeader 44
#define recordName "/last.wav"

AudioIO::AudioIO(){
}
void AudioIO::setPinOut(int bclk, int lrc, int dout){
  out.setPinout(bclk, lrc, dout);
}
void AudioIO::setVolume(int vol){
  out.setVolume(vol);
}
void AudioIO::setMicrophone(adc1_channel_t adc){
  mic_pin= adc;
}
void AudioIO::Play(const char *filename){
	out.connecttoFS(SPIFFS, filename);
	do {
		out.loop();
	}while (isPlaying());
}
void AudioIO::ReproduceRecord(){
	out.connecttoFS(SPIFFS, recordName);
	do{
		out.loop();
	}while (isPlaying());
}
void AudioIO::fillSamples(byte* buff, int buff_len){
  int flash_wr_size= 0;
  
  while (flash_wr_size < buff_len) {
        unsigned long t1= micros();

        buff[flash_wr_size]= (byte)(adc1_get_raw(mic_pin)>>1);//get sample from ADC
        
        flash_wr_size++; //samples recorder increment

        while (micros()-t1<=64);//wait for get next sample
  }
}
void AudioIO::RecordBuff(int seconds, byte* buff, int buff_len, bool pos, int frequency){
  int numSamples= seconds*frequency;//Samples of the record
  
  formatHeader(header, numSamples+buff_len, 1, frequency, 8);//header of wav file, size of the file (RecordSamples and BuffSamples), num channels, sampling rate, bit per Sample
  
  File rec= SPIFFS.open(recordName, FILE_WRITE);
  rec.write(header, wavHeader);

  if (pos) rec.write(buff, buff_len);

  int flash_wr_size= 0;//Flash size

  while (flash_wr_size < numSamples) {
        unsigned long t1= micros();
        
        byte sample= (byte)(adc1_get_raw(mic_pin)>>1);//get sample from ADC
        rec.write(sample);//write it on file

        flash_wr_size++; //samples recorder increment
        while (micros()-t1<=64);//wait for get next sample
  }
  if (!pos) rec.write(buff, buff_len);
  
  rec.close();
}
void AudioIO::Record(int seconds, int frequency){
  int numSamples= seconds*frequency;
  
	formatHeader(header, numSamples, 1, frequency, 8);
	
	File rec= SPIFFS.open(recordName, FILE_WRITE);
	rec.write(header, wavHeader);

  int flash_wr_size= 0;

	while (flash_wr_size < numSamples) {
        unsigned long t1= micros();
        
        byte sample= (byte)(adc1_get_raw(mic_pin)>>1);//get sample from ADC
        rec.write(sample);//write it on file

        flash_wr_size++; //samples recorder increment
        while (micros()-t1<=64);//wait for getting next sample
    }
  rec.close();
}
bool AudioIO::isPlaying(){
	return out.isRunning();
}
void AudioIO::formatHeader(byte* header, int wavSize, int channels, int sample_rate, int BitsPerSample){
  //wavSize Numero campioni
  int byte_rate= (sample_rate*channels*BitsPerSample)>>3;
  int block_align= channels * BitsPerSample>>3;
  int SubChunk2Size= (wavSize*channels*BitsPerSample)>>3;
  int ChunkSize= 36 + SubChunk2Size;
  
  
  header[0] = 'R';//chuck id
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  header[4] = (byte)(ChunkSize & 0xFF);//chuck size
  header[5] = (byte)((ChunkSize >> 8) & 0xFF);
  header[6] = (byte)((ChunkSize >> 16) & 0xFF);
  header[7] = (byte)((ChunkSize >> 24) & 0xFF);
  header[8] = 'W';//Format, big endian form
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';//  Subchunk1ID 
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;//16 for PCM.  This is the size of the rest of the Subchunk which follows this number.
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;//PCM = 1 (i.e. Linear quantization)
  header[21] = 0x00;
  header[22] = channels;//Num channels: Mono = 1, Stereo = 2, etc.
  header[23] = 0x00;
  header[24] = (byte)(sample_rate & 0xFF);//Sample rate little endian right_to_left
  header[25] = (byte)((sample_rate>>8) & 0xFF);//Sample rate little endian right_to_left
  header[26] = (byte)((sample_rate>>16) & 0xFF);
  header[27] = (byte)((sample_rate>>24) & 0xFF);
  header[28] = (byte)(byte_rate & 0xFF);//Bite rate
  header[29] = (byte)((byte_rate>>8) & 0xFF);//little endian right_to_left
  header[30] = (byte)((byte_rate>>16) & 0xFF);
  header[31] = (byte)((byte_rate>>24) & 0xFF);
  header[32] = (byte)(block_align & 0xFF);//block align NumChannels * BitsPerSample/8 The number of bytes for one sample including all channels.
  header[33] = (byte)((block_align>>8) & 0xFF);
  header[34] = (byte)(BitsPerSample & 0xFF);//bit per sample
  header[35] = (byte)((BitsPerSample>>8) & 0xFF);
  header[36] = 'd'; //Subchunk2ID 
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(SubChunk2Size & 0xFF);//Subchunk2Size  NumSamples * NumChannels * BitsPerSample/8
  header[41] = (byte)((SubChunk2Size >> 8) & 0xFF);
  header[42] = (byte)((SubChunk2Size >> 16) & 0xFF);
  header[43] = (byte)((SubChunk2Size >> 24) & 0xFF);
}
