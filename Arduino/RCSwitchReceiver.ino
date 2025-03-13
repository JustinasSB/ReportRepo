#define DATA_PIN 19
#define CS_PIN 21
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN,HIGH);
  Serial.println("decode test");
  mySwitch.enableReceive(digitalPinToInterrupt(DATA_PIN));
}

void loop() {
  if (mySwitch.available()) {
    
    Serial.print("Received ");
    Serial.print( mySwitch.getReceivedValue() );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    mySwitch.resetAvailable();
  }
}
