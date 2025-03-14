#include "esphome.h"

const int BUFFER_SIZE = 2048;
volatile signed long pulseDurations[BUFFER_SIZE];
volatile signed long pulseDuration = 0;
volatile bool newData = false;
volatile int pulseCount = 0;
volatile unsigned long lastPulse = 0;
volatile unsigned long lastTime = 0;
volatile unsigned long TIMEOUT = 10;
volatile bool invert = false;
volatile int IOpin = 0;

void IRAM_ATTR handleInterrupt() {
  unsigned long currentTime = micros();
  pulseDuration = currentTime - lastTime;
  lastTime = currentTime;
  if(digitalRead(IOpin)==HIGH){
    pulseDuration = ~pulseDuration + 1;
  }
  else{
    pulseDuration = pulseDuration;
  }
  if(invert){
    pulseDuration = ~pulseDuration + 1;
  }
  if(pulseCount < BUFFER_SIZE){
    pulseDurations[pulseCount++] = pulseDuration;
    lastPulse = currentTime;
  }
  if(pulseCount == BUFFER_SIZE){
    newData = true;
  }
}
void rf_receiver_task( void * pvParameters ){
  attachInterrupt(digitalPinToInterrupt(IOpin), handleInterrupt, CHANGE);
  while(1){
    unsigned long currentTime = micros();
    unsigned long currentMillis = millis();
    if(newData){
      std::string temp = "[";
      for(int i=0;i<pulseCount;i++){
        if(i%60==0){
          ESP_LOGD("rf_receiver", "Pulses[%d]: %s ", i, temp.c_str());
          temp = "";
        }
        temp.append(std::to_string(pulseDurations[i]));
        temp.append(", ");
      }
      temp.append("]");
      ESP_LOGD("rf_receiver", "Pulses[Final]: %s µs", temp.c_str());
      pulseCount=0;
      newData=false;
    }
    if((currentMillis - (lastPulse)/1000 > TIMEOUT) && !newData){
      if(pulseCount<10) {
        pulseCount = 0;
        newData = false;
        lastPulse = micros();
      }
      else{
        std::string temp = "[";
        for(int i=0;i<pulseCount;i++){
          if(i%60==0 && i!=0){
            ESP_LOGD("rf_receiver", "Pulses[%d]: %s ", i, temp.c_str());
            temp = "";
          }
          temp.append(std::to_string(pulseDurations[i]));
          temp.append(", ");
        }
        temp.append("]");
        ESP_LOGD("rf_receiver", "Pulses[Final]: %s µs", temp.c_str());
        pulseCount = 0;
        newData = false;
        lastPulse = micros();
      }
    }
  }
  
}

class CustomRFReceiver : public PollingComponent {
public:
    int pin;
    bool inverter;
    long idle;
    CustomRFReceiver(int pin, bool inv, long idle) : PollingComponent(50), pin(pin), inverter(inv), idle(idle) {}

    void setup() override {
        ::IOpin = pin;
        ::invert = inverter;
        ::TIMEOUT = idle;
        pinMode(IOpin, INPUT);
        xTaskCreatePinnedToCore(rf_receiver_task,"RF_Receiver_Task",4096,NULL,1,NULL,1);
    }
    void update() override{
      
    }
};
