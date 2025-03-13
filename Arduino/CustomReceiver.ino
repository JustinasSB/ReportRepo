#define DATA_PIN 19
#define CS_PIN 21

volatile unsigned long lastTime = 0;
volatile unsigned long lastPrint = 0;
volatile unsigned long lastPulse = 0;
volatile unsigned long pulseDuration = 0;
volatile unsigned int printedlines = 0;
const int maxPulses = 986;
volatile unsigned long pulseDurations[maxPulses];
volatile int pulseCount = 0;
volatile bool newData = false;
const unsigned long TIMEOUT = 1000;

void IRAM_ATTR handleInterrupt(){
  unsigned long currentTime = micros();
  pulseDuration = currentTime - lastTime;
  lastTime = currentTime;
  lastPulse = currentTime;
  if(pulseCount < maxPulses){
    pulseDurations[pulseCount++] = pulseDuration;
  }
  if(pulseCount == maxPulses){
    newData = true;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN,HIGH);
  Serial.println("decode test");
  pinMode(DATA_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(DATA_PIN), handleInterrupt, CHANGE);
}

void loop() {
  unsigned long currentTime = micros();
  unsigned long currentMillis = millis();

  if(newData){
    Serial.println("Pulse durations");
    Serial.println("");
    Serial.print("[");
    for(int i=0;i<pulseCount;i++){
      Serial.print(pulseDurations[i]);
    }
    Serial.print("]");
    pulseCount=0;
    newData=false;
    
  }
  if(currentMillis - (lastPulse/1000) > TIMEOUT && !newData){
    pulseCount = 0;
    newData = false;
    lastPulse = micros();
  }
  delay(1000);
}
