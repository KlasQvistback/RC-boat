#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#define RF69_FREQ 433.0
#define DEST_ADDRESS   30
#define MY_ADDRESS     86
//#if defined(ADAFRUIT_FEATHER_M0
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
//#endif

RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);
 
int throttlePin = A0;
int potVal = 0;
int steeringPin = A1; //change

int diff = 0;
int oldAngle = 0;
int newAngle = 0;
int blinkTime = 1000;
float TX_battery = 0;
int TX_pin = 12;
int TX_BAT_pin = 11;
int TX_BAT_LEDstate = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
int TX_LEDstate = 1;
unsigned long currentTime = 0;
unsigned long lastReceivedTime = 0;
int powerPin = 10;
//12 - 10 LEDPins
 
 
struct dataStruct {
 bool connection_TX;
 int throttle;
 int angle;
 float batteryLevel;
 int RX_BAT_LEDstate;
} values;
 
void setup()
{
 Serial.begin(115200);
 pinMode(throttlePin, INPUT);
 pinMode(LED_BUILTIN, OUTPUT);
 pinMode(TX_LEDstate, OUTPUT);
 pinMode(powerPin, OUTPUT);
 
 if (!rf69_manager.init())
 {
   Serial.println("RFM69 radio init failed");
   while (1);
 }
 
 if (!rf69.setFrequency(RF69_FREQ))
 {
   Serial.println("setFrequency failed");
 }
 
 rf69.setTxPower(20, true);
 uint8_t key[] = { 0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                 };
 rf69.setEncryptionKey(key);
 
 
 values.connection_TX = 0;
 Serial.print("Startad");
 digitalWrite(powerPin, 1);
 rf69_manager.setRetries(1);
}
 
void loop(){
 currentTime = millis();
 if(currentTime - lastReceivedTime > 500){
   lastReceivedTime = currentTime;
   digitalWrite(TX_pin,0);
   Serial.println("lost connection");
 }
 updateData();
  if (rf69_manager.sendtoWait((uint8_t *)&values, sizeof(values), DEST_ADDRESS)) {
   lastReceivedTime = millis();
   uint8_t len = sizeof(values);
   uint8_t from;
   Serial.println("Connected...");
   digitalWrite(LED_BUILTIN, 1);
   values.connection_TX = 1;
   digitalWrite(TX_pin, values.connection_TX);
  
 }
 else {
   Serial.println("Failed to send...");
   digitalWrite(LED_BUILTIN, 0);
   delay(200);
   digitalWrite(LED_BUILTIN, 1);
   values.connection_TX = 0;
 }
 //Was previously "A7"
  TX_battery = (analogRead(A5) * 3.3 * 2.0 * 1.1 / 1024);
 Serial.print(TX_battery);
 Serial.print("\t");
 //tx batteryled
 if (TX_battery < 3.6 && TX_battery > 3.3) {
   currentMillis = millis();
   if (currentMillis - previousMillis >= blinkTime) {//if the time since the arduino last started is equal to what time it was before then toggle LEDState
     previousMillis = currentMillis;//saves the time since
     TX_BAT_LEDstate = ! TX_BAT_LEDstate; //Toggle the LEDState
   }
   digitalWrite(TX_BAT_pin, TX_BAT_LEDstate);
 }
 if (TX_battery < 3.3) {
   digitalWrite(TX_BAT_pin, 1);
 }
}
 
void updateData() {
 //batteryLevel 
 values.batteryLevel = analogRead(A5) * 3.3 * 2.0 * 1.1 / 1024;
 
 //throttle
 values.throttle = analogRead(throttlePin);
 values.throttle = map(values.throttle, 0, 1023, 255, 0);
 Serial.println(values.throttle);
 //hysteresis angle
 newAngle = analogRead(steeringPin);
 newAngle = map(newAngle, 0, 1023, 140, 40);
 diff = abs(oldAngle - newAngle);
 Serial.print(newAngle);
 Serial.print("\t");
 Serial.print(oldAngle);
 Serial.print("\t");
 Serial.print(diff);
 Serial.print("\t");
 if(diff < 3){
   values.angle = oldAngle;
  
   Serial.println("gammalt värde");
  
 }
 if(diff > 3 ){
  
   values.angle = newAngle;
   oldAngle = newAngle;
   Serial.print(newAngle);
   Serial.print("\t");
   Serial.print(oldAngle);
   Serial.print("\t");
   Serial.println("nytt värde");
 }
  Serial.println(values.angle);
 
 /*
 //throttle
 values.throttle = analogRead(throttlePin);
 for(int i = 0; i < 10; i++){
   averageThrottle = averageThrottle + values.throttle;
 }
 values.throttle/10;
  //servo
 values.angle = analogRead(steeringPin);
 for(int i = 0; i < 1000; i++){
   averageAngle = averageAngle + values.angle;
 }
 values.angle/1000;
 */
 
}