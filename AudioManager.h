#include <driver/i2s.h>

//44.1 kHz × 16 × 2 = 1.4112 MHz
#define I2S_SAMPLE_RATE 44100

i2s_config_t i2s_config = {
  .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
  .sample_rate = 44100,
  .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
  //.communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_STAND_MSB),
  .intr_alloc_flags = 0, // default interrupt priority
  .dma_buf_count = 2,
  .dma_buf_len = 256, // this is in samples, not bytes
  .use_apll = false,
  .tx_desc_auto_clear = true // avoiding noise in case of data unavailability
};

void SetupAudio() {
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, NULL); //  use internal codec to DAC
  i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN); // right channel on DAC is pin 25
  // think i need to pull up this pin to power up the external amp
  // https://github.com/leighmurray/LilyGo_Txx
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);
}

void PlayAudioBuffer(byte outputAudioBufferLeft[256], byte outputAudioBufferRight[256]){
  size_t i2s_bytes_write;
  byte samplesData[512];

  // for each set of samples, write the right channel, then the left channel
  for (int i=0; i<256; i+=2){
    int sampleDataIndex = i*2;
    memcpy(&samplesData[sampleDataIndex], &outputAudioBufferRight[i], 2);
    memcpy(&samplesData[sampleDataIndex+2], &outputAudioBufferLeft[i], 2);
  }

  i2s_write(I2S_NUM_0, samplesData, 512, &i2s_bytes_write, 100);
}
