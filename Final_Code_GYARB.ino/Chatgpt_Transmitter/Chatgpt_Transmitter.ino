#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

#define Transmitter_ADDRESS 86
#define Receiver_ADDRESS 30

RH_RF69 rf69(8, 3);
RHReliableDatagram rf69_manager(rf69, Transmitter_ADDRESS);

int PotS = A0;
int PotE = A1;

struct PotData{
  bool connection_TX;
  int servo;
  int esc;
} values;

void setup(){
  Serial.begin(115200);
  if (!rf69_manager.init())
 {
   Serial.println("RFM69 radio init failed");
   while (1);
 }
 
 if (!rf69.setFrequency(433.0))
 {
   Serial.println("setFrequency failed");
 }
 
 rf69.setTxPower(20, true);
 uint8_t key[] = { 0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                 };
 rf69.setEncryptionKey(key);

 values.connection_TX = 0;
 Serial.print("Transmitter startad");
 rf69_manager.setRetries(1);
}

void loop(){
 //Throttle
 values.esc = analogRead(PotE);
 values.esc = map(values.esc, 0, 1023, 1000, 2000);
 Serial.println(values.esc);

 //Steering
 values.servo = analogRead(PotS);
 values.servo = map(values.servo, 0, 1023, 0, 180);
 Serial.println(values.servo);

 if (rf69_manager.sendtoWait((uint8_t *)&values, sizeof(values), Receiver_ADDRESS)) {
   uint8_t len = sizeof(values);
   uint8_t from;
   Serial.println("Connected...");
   values.connection_TX = 1;
 }
 else {
   Serial.println("Failed to send...");
   digitalWrite(LED_BUILTIN, 0);
   delay(200);
   digitalWrite(LED_BUILTIN, 1);
   values.connection_TX = 0;
 }
}
