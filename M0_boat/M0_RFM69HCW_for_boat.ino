//The boats code
//Libraies
#include <SPI.h> //Needed for the other libraires
#include <RH_RF69.h>  //Needed for the other libraries
#include <RHReliableDatagram.h> //For sending data packets reliably
#include <Servo.h> //For controling the steering servo
#include <Adafruit_MotorShield.h> //For controlling the motor

//Defining the frequency of the RFM69HCW radio
#define RF69_FREQ 433.0

#define MY_ADDRESS     30//The address of the boat
#define DEST_ADDRESS   50//The handcontrollers code

//#if defined(ADAFRUIT_FEATHER_M0)
#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
//#endif
RH_RF69 rf69(RFM69_CS, RFM69_INT);

RHReliableDatagram rf69_manager(rf69, MY_ADDRESS); //Assigning the boats address this board

//Creating the motor shield object
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
//The motor is connected to the M1 port on the dc motor + stepper featherwing
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

int steeringPin = 6; //the pin the steering servo is connected to
Servo servo; //Creating the servo object

//Variables for keeping track of the time
unsigned long lastReceivedTime = 0;
unsigned long currentTime = 0;

//Defining the structure of the struct packet
struct structPacket {
  int Speed; //Controling the boats speed and direction
  int steering; //Controlling the steering angle of the boat
} values; //The prefix used to change the data in the struct

//Values to fall back on if connection is lost
void resetData() {
  myMotor->run(RELEASE);  //Taking the motor out of gear

  values.steering = 90;   //Steering straight
  values.Speed = 127;     //Replicating the neutral posistion of the joystick
}
void setup() {
  Serial.begin(115200);
  servo.attach(steeringPin); //Attaching the servo object to the pin the servo is connected to
  AFMS.begin(); //Initiating the motor
  myMotor->setSpeed(0); //Setting the starting speed to zero

  if (!rf69_manager.init())  //Initiating the RFM69HCW chip
  {
    Serial.println("RFM69 radio init failed"); //If it didn´t initiate
    while (1);
  }

  if (!rf69.setFrequency(RF69_FREQ))  //Making sure the radio frequenzy can be used by the RFM69HCW chip
  {
    Serial.println("setFrequency failed");//If it can´t use the frequenzy
  }

  rf69.setTxPower(20, true); //Special command needed for HCW version of the RFM69

  //The encyption key
  uint8_t key[] = { 0x04, 0x02, 0x00, 0x06, 0x09, 0x04, 0x02, 0x00,
                    0x04, 0x02, 0x00, 0x06, 0x09, 0x04, 0x02, 0x00
                  };
  rf69.setEncryptionKey(key);  //Uppdating the encryption key
}

void loop() {

  currentTime = millis(); //Cheking the current time


  if (currentTime - lastReceivedTime > 500) { //If too long time passes without an answer the connection is lost
    currentTime = lastReceivedTime; //Saving the time for future refrence
    resetData(); //Shuting of the motor and stop turning when connection is lost
  }
  uint8_t len = sizeof(values); //Making sure the message is the correct size
  uint8_t from;    //Making sure the message has the correct address

  //When reciving a message, sends ackowledgement to the handcontroller
  rf69_manager.recvfromAckTimeout((uint8_t *)&values, &len, 10, &from);

  if (rf69_manager.available()) {
    lastReceivedTime = millis(); //Uppdating the current time after an ackoweldegement is sent
    Serial.print(values.steering);
    Serial.print("\t");
    Serial.println(values.Speed);
  }
  Serial.println("connected...");

  //To filter out inacurate signals when not turning
  if (values.steering > 88 && values.steering < 92) {
    servo.write(90); //Steering straihgt
  }
  else { //If turning more than 2 degrees, then actually turn the boat
    servo.write(values.steering);
  }


  //When the joystick controling the speed is in its neutral position
  if (values.Speed > 125 && values.Speed < 130) {
    myMotor->run(RELEASE);
  }
  if (values.Speed > 130) {               //When given throttle forwards
    myMotor->run(FORWARD);                //Set motor to go forwards
    myMotor->setSpeed(values.Speed - 130); //-130 so there is a soft start when going forwards
  } 
  
  if (values.Speed < 125) {                 //When given throttle backwords
    myMotor->run(BACKWARD);                //Set motor to run backwards
    myMotor->setSpeed(125 - values.Speed); //125-Speed to start reversing slowly and reverse faster and faster
  }
}