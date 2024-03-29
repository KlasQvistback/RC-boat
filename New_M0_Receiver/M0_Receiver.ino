#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <Servo.h>
#include <Adafruit_MotorShield.h>
#define RF69_FREQ 433.0
#define MY_ADDRESS     30
#define DEST_ADDRESS 86
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
 
RH_RF69 rf69(RFM69_CS, RFM69_INT);
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);
 
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);// Select which 'port' M1, M2, M3 or M4. In this case, M1
Servo servo;
 
int servoPin = 11;//thilde, connect to car
unsigned long lastReceivedTime = 0;
unsigned long currentTime = 0;
 
 
struct dataStruct {
 bool connection_TX;
 int throttle;
 int angle;
 float batteryLevel;
 int RX_BAT_LEDstate;
} values;
 
void setup() {
 Serial.begin(115200);
 pinMode(servoPin, OUTPUT);
 servo.attach(servoPin);
 pinMode(LED_BUILTIN, OUTPUT);
 AFMS.begin();
 myMotor->setSpeed(0);
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
}
 
void loop() { //LED for not_connected is in setup, green light in loop
 currentTime = millis();
 
 if(currentTime - lastReceivedTime > 500){
   lastReceivedTime = currentTime;
   myMotor->run(RELEASE);
   values.angle = 90;
   Serial.print("Lost connection");
 }
 
 uint8_t len = sizeof(values);
 uint8_t from;
  //rf69_manager.recvfromAck((uint8_t *)&values, &len, &from);
 rf69_manager.recvfromAckTimeout((uint8_t *)&values, &len, 1, &from);
 if (values.connection_TX == 0){
   Serial.println("No reply, Waiting...");
  }
  
  if (rf69_manager.available()) {
   lastReceivedTime = millis();
   Serial.print(values.angle,DEC);
   Serial.print("\t");
   Serial.print(values.throttle);
   Serial.print("\t");
   Serial.print(values.batteryLevel);
   Serial.print("\t");
   Serial.print(values.connection_TX);
   Serial.print("\t");
 
   servo.write(values.angle);
  
  
   Serial.println("connected...");
  
  
   if(values.throttle > 125 && values.throttle < 130){
   myMotor->run(RELEASE);
   }
 
   if(values.throttle > 130){
     myMotor->run(BACKWARD);
     myMotor->setSpeed(values.throttle);
   }
 
   if(values.throttle < 125){
     myMotor->run(FORWARD);
     myMotor->setSpeed(150-values.throttle);
   }  
 }
}
