/*
 * This is an example to read analog data at high frequency using the I2S peripheral
 * Run a wire between pins 27 & 32
 * The readings from the device will be 12bit (0-4096) 
 */
#include <driver/i2s.h>

#define I2S_SAMPLE_RATE 78125
#define ADC_INPUT ADC1_CHANNEL_7 //pin 32
#define OUTPUT_PIN 5
#define OUTPUT_VALUE 3800
#define READ_DELAY 100 //microseconds

uint32_t adc_reading;
byte ADC_flag=0;
unsigned long time2=0;
uint32_t ADC_table[1000];

void i2sInit()
{
   i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate =  I2S_SAMPLE_RATE,              // The format of the signal using ADC_BUILT_IN
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 8,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
   };
   i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
   i2s_set_adc_mode(ADC_UNIT_1, ADC_INPUT);
   i2s_adc_enable(I2S_NUM_0);
}

void reader(void *pvParameters) {
  for(uint16_t k=0;k<1000;k++){
    ADC_table[k]=0;
  }
  
  while(1){
    if(ADC_flag==1) break;
    delayMicroseconds(1);
  }
  
  time2=0;
  
  uint32_t read_counter = 0;
  uint64_t read_sum = 0;
// The 4 high bits are the channel, and the data is inverted
  uint16_t offset = (int)ADC_INPUT * 0x1000 + 0xFFF;
  size_t bytes_read;
  uint16_t gg=1000;

  
  while(1){
    uint16_t buffer[2] = {0};
    i2s_read(I2S_NUM_0, &buffer, sizeof(buffer), &bytes_read, 15);
    //Serial.printf("%d  %d\n", offset - buffer[0], offset - buffer[1]);
    
    if (bytes_read == sizeof(buffer)) {
      read_sum += offset - buffer[0];
      read_sum += offset - buffer[1];
      ADC_table[read_counter]=(2*offset-buffer[0]-buffer[1])/2;
      read_counter++;
    } else {
      Serial.println("buffer empty");
    }
    if (read_counter == gg) {
      adc_reading = read_sum / 2;
      ADC_flag=0;
      //Serial.printf("avg: %d millis: ", adc_reading);
      //Serial.println(millis());
      read_counter = 0;
      read_sum = 0;
      i2s_adc_disable(I2S_NUM_0);
      i2s_adc_enable(I2S_NUM_0);
    }
  }
  
}




void setup() {
  Serial.begin(115200);
  // Put a signal out on pin 
//  uint32_t freq = ledcSetup(0, I2S_SAMPLE_RATE, 10);
//  Serial.printf("Output frequency: %d\n", freq);
//  ledcWrite(0, OUTPUT_VALUE/4);
//  ledcAttachPin(OUTPUT_PIN, 0);
  // Initialize the I2S peripheral
  i2sInit();
  // Create a task that will read the data
  xTaskCreatePinnedToCore(reader, "reader", 2048, NULL, 1, NULL, 1);
}




void loop() {
  delay(1);
   
  unsigned long time0=0;
  unsigned long time1=0;
  
  time0=micros();
  ADC_flag=1;
  
  while(1){
    if(ADC_flag==0) break;
    delayMicroseconds(1);
  }
  time1=micros();
  time2=(time1-time0);
  
  Serial.print("ADC reading:  ");
  Serial.println(adc_reading);
  Serial.println(time2);
//  for(uint16_t hh=0;hh<1000;hh++){
//    Serial.println(ADC_table[hh]);
//  }
  delay(1000);
}
