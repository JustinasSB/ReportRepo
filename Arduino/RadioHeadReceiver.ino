#include <RH_ASK.h>
#include <SPI.h>
#define DATA_PIN 21
#define CS_PIN 16
RH_ASK rf_driver(2000,21,-1,-1);

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN,HIGH);
  if(!rf_driver.init())
  {
    Serial.println("RF driver initialization failed");
    while(1){
      Serial.println("halted");
      delay(10000);
    }
  }
  Serial.println("Initialized");
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);
  if(rf_driver.recv(buf, &buflen)){
    buf[buflen]='\0';
    Serial.print("received: ");
    Serial.println((char*)buf);
  }
}
