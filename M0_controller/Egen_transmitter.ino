#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#define RF69_FREQ 434.0
#define DEST_ADDRESS   30
#define MY_ADDRESS     86
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

int throttle = A0;
int potVal = 0;
int steering = A1;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long currentTime = 0;
unsigned long lastReceivedTime = 0;

struct dataStruct{
 int throttle;
 int steering;
} values;

void setup(){
 Serial.begin(115200);
 pinMode(throttle, INPUT);

  if (!rf69_manager.init()){
   Serial.println("RFM69 radio init failed");
   while (1);
 }
 
 if (!rf69.setFrequency(RF69_FREQ)){
   Serial.println("setFrequency failed");
 }
 
 rf69.setTxPower(14, true);
 uint8_t key[] = { 0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                 };
 rf69.setEncryptionKey(key);
 
 Serial.print("Startad");
 rf69_manager.setRetries(1);
}

void loop(){
 currentTime = millis();
 if(currentTime - lastReceivedTime > 500){
   lastReceivedTime = currentTime;
   Serial.println("lost connection");
 }
  updateDATA();
  if (rf69_manager.sendtoWait((uint8_t *)&values, sizeof(values), DEST_ADDRESS)) {
   lastReceivedTime = millis();
   uint8_t len = sizeof(values);
   uint8_t from;
   Serial.println("Connected...");
  }
  else {
   Serial.println("Failed to send...");
 }

}

void updateData() {
 values.throttle = analogRead(throttle);
 values.throttle = map(values.throttle, 0, 1023, 255, 0);
 Serial.println(values.throttle);

 values.steering = analogRead(steering);
 values.steering = map(values.steering, 0, 1023, 180, 0);
 Serial.println(values.steering);
}