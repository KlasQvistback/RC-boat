#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include "Servo.h"

#define Transmitter_ADDRESS 86
#define Receiver_ADDRESS 30

RH_RF69 rf69(8, 3);
RHReliableDatagram rf69_manager(rf69, Receiver_ADDRESS);

Servo SERVO;
Servo ESC;

struct PotData{
  bool connection_TX;
  int servo;
  int esc;
} values;

void setup(){
  ESC.attach(9);
  SERVO.attach(10);
  Serial.begin(115200);
  if (!rf69_manager.init()){
   Serial.println("RFM69 radio init failed");
   while (1);
 }
 
 if (!rf69.setFrequency(433.0)){
   Serial.println("setFrequency failed");
 }
 
 rf69.setTxPower(20, true);
 uint8_t key[] = { 0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                 };
 rf69.setEncryptionKey(key);
}

void loop(){
 uint8_t len = sizeof(values);
 uint8_t from;
 //rf69_manager.recvfromAck((uint8_t *)&values, &len, &from);
 rf69_manager.recvfromAckTimeout((uint8_t *)&values, &len, 1, &from);
 if (values.connection_TX == 0){
   Serial.println("No reply, Waiting...");
 }

 if (rf69_manager.available()){
  Serial.println(values.servo);
  SERVO.write(values.servo);
  Serial.println(values.esc);
  ESC.write(values.esc);
  
  if (values.esc > 1500) {  // Only send signal to ESC if value is above 1500
    digitalWrite(10, HIGH);
    delayMicroseconds(values.esc);
    digitalWrite(10, LOW);
  }
 
  Serial.print("Connected...");
 }
}
