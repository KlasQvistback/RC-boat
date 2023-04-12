#include "SPI.h"
#include "RH_RF69.h"
#include <Servo.h>

int potS;
int servoValue;

int potE;
int escValue;

Servo ESC;
Servo myServo;

void setup(){
 ESC.attach(9);
 myServo.attach(10);
 Serial.begin(115200);
}

void loop() {
  potS = analogRead(A0);
  servoValue = map(potS, 0, 1023, 0, 180);
  ESC.write(servoValue);

  potE = analogRead(A1);
  escValue = map(potE, 0, 1023, 1000, 2000);
  myServo.write(escValue);

  if (escValue > 1500) {  // Only send signal to ESC if value is above 1500
    digitalWrite(10, HIGH);
    delayMicroseconds(escValue);
    digitalWrite(10, LOW);
  }

  Serial.print("Servo potentiometer value: ");
  Serial.println(potS);
  Serial.print("esc potentiometer value: ");
  Serial.println(potE);
  Serial.print("Servo value: ");
  Serial.println(servoValue);
  Serial.print("ESC value: ");
  Serial.println(escValue);
  
  delay(50);
}
