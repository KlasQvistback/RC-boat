//Handcontroller code

//Libraries
#include <SPI.h> //Needed for the other libraries
#include <RH_RF69.h> //For controlling the RFM69HCW radio chip
#include <RHReliableDatagram.h> //For sending data packets reliably

//Defining the frequenzy of the RFM69HCW chip
#define RF69_FREQ 433.0

#define DEST_ADDRESS   30 //The address of the boat
#define MY_ADDRESS     50 //The address of the handcontroller


//#if defined(ADAFRUIT_FEATHER_M0)
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
//#endif
RH_RF69 rf69(RFM69_CS, RFM69_INT); // uppdating the defined pins

RHReliableDatagram rf69_manager(rf69, MY_ADDRESS); //Giving this board the handcontroller address

//Variables for the joysticks
int SpeedPin = A0;  //The pin for the joystick controlling the boats speed
int steeringPin = A1; //The pin for the joystick controlling the boats steering


//Variables for keeping time
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long currentTime = 0;
unsigned long lastReceivedTime = 0;


//Defining the structure of the struct packet
struct structPacket {
  int Speed;
  int steering;
} values; //Values is the "key" to uppdate data in the struct packet

void setup()
{
  Serial.begin(115200);

  //Setting the correct pin modes for all the used pins
  pinMode(SpeedPin, INPUT); //For the pin connected to the joystick controling Speed
  pinMode(steeringPin, INPUT); //For the pin connected to the joystick controling Steering

  if (!rf69_manager.init())  //Initiating the RFM69HCW chip
  {
    Serial.println("RFM69 radio init failed"); //If it didn´t initiate
    while (1);
  }

  if (!rf69.setFrequency(RF69_FREQ))  //Making sure the radio frequenzy can be used by the RFM69HCW chip
  {
    Serial.println("setFrequency failed");//If it can´t use the frequenzy
  }

  rf69.setTxPower(20, true); //Special command needed for HCW version of RFM69

  //The encyption key
  uint8_t key[] = { 0x04, 0x02, 0x00, 0x06, 0x09, 0x04, 0x02, 0x00,
                    0x04, 0x02, 0x00, 0x06, 0x09, 0x04, 0x02, 0x00
                  };
  rf69.setEncryptionKey(key); //Uppdating the encryption key
}

void loop() {

  currentTime = millis(); //Cheking the time

  //Making sure the code doesn´t get stuck when connection is lost
  if (currentTime - lastReceivedTime > 500) { //If too long time passes without an answer the connection is lost
    currentTime = lastReceivedTime; //Saving the time for future refrence
    Serial.println("lost the connection to the boat");
  }

  //uppdating the data of the struct packet
  //Speed value
  values.Speed = map(analogRead(SpeedPin), 0, 1023, 255, 0); //Reading the joystick controling the speeds value and converting it to an acceptable range for the motor, 0 to 255
  values.Speed = constrain(values.Speed, 0, 250);             //Constraing the value to be within the range
  //Steering value
  values.steering = map(analogRead(steeringPin), 0, 1023, 180, 0); //Reading the steering joysticks value and converting to the steering servos range, 0 to 180
  values.steering = constrain(values.steering, 25 , 155);           //Constraing the steering angle so the rudder doesnt hit the hull of the boat.

  if (rf69_manager.sendtoWait((uint8_t *)&values, sizeof(values), DEST_ADDRESS)) { //When it is the handcontrollers turn, send the struct packet
    lastReceivedTime = millis(); //Save the time the recived message
    uint8_t len = sizeof(values); //Making sure the message is the correct size
    uint8_t from;    //Making sure the message has the correct address

    Serial.println("Still connected   ");
  }

  else { //If a message fails to send
    Serial.println("-Failed to send data packet   ");
    //Do nothing but wait for an acknowledgement
  }
}
