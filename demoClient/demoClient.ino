//LoRa libs and variables
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;
float frequency = 868.0;

//DHT11 Sensor libs and variables
// Documentation http://playground.arduino.cc/Main/DHTLib
#include <dht.h>

dht DHT;
const int DHTpin = 7;

// OpenPIR sensor libs and variables

const int digitalPIR = 8;

//Motor Stuff
#include <PWMServo.h>
PWMServo myservo;
const int motorPin = 10;


void loraInit() {
  rf95.init();
  rf95.setFrequency(frequency); // Setup ISM frequency
  rf95.setTxPower(13); // Setup Power,dBm
  rf95.setSpreadingFactor(7); // Setup Spreading Factor (6 ~ 12)
  rf95.setSignalBandwidth(125000);   // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  rf95.setCodingRate4(5);  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
}

unsigned long prevTime = millis();
bool PIRtoggle = false;
uint8_t count = 0;

//LDR
const int LDRLpin = A1;
const int LDRRpin = A2;

int LDRLeft = 0;
int LDRRight = 0;
int calibL = 0;
int calibR = 0;
int motorPos = 60;
const int positionInc = 1;
const int threshold = 10;
const int numReadings = 50;
int readings[numReadings];      // the readings from the analog input


void setup() {
  Serial.begin(9600);
  pinMode(digitalPIR, INPUT);
  pinMode(DHTpin, INPUT);
  myservo.attach(motorPin);
  myservo.write(motorPos);
  delay(50);
  loraInit();
  calibL = analogRead(LDRLpin);
  calibR = analogRead(LDRRpin);
  
}

void smoothLDR() {
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    LDRLeft =  (LDRLeft + analogRead(LDRLpin)-calibL);
    LDRRight =  (LDRRight + analogRead(LDRRpin)-calibR);
    delay(1);
  }
  
  LDRLeft = LDRLeft / numReadings;
  LDRRight = LDRRight / numReadings;
}

void checkMotormotorPosition() {
  if ((LDRLeft > LDRRight) && (LDRLeft - LDRRight > threshold)) {
    motorPos = motorPos - positionInc;
  } else if ((LDRRight > LDRLeft) && (LDRRight - LDRLeft > threshold)) {
    motorPos = motorPos + positionInc;
  }
  //Limit checking.
  if (motorPos > 119) {
    motorPos = 119;
  } else if (motorPos <= 5) {
    motorPos = 5;
  }
}

void loop() {

  smoothLDR();
  checkMotormotorPosition();
  
 
  myservo.attach(motorPin);
  myservo.write(motorPos);

  if (digitalRead(digitalPIR) == HIGH && PIRtoggle == false) {
    PIRtoggle = true;
    count++;
  } else if (digitalRead(digitalPIR) == LOW) {
    PIRtoggle = false;
  }
  
  DHT.read11(DHTpin);
  uint8_t dataValues[5];
  uint8_t temperature = (uint8_t)DHT.temperature;
  uint8_t humidity = (uint8_t)DHT.humidity;
  dataValues[0] = 55;
  dataValues[1] = temperature;
  dataValues[2] = humidity;
  dataValues[3] = (uint8_t)count;
  dataValues[4] = (uint8_t)motorPos;
  Serial.println(String(dataValues[1]) + "\t" + String(dataValues[2]) + "\t" + String(dataValues[3]) + "\t" + String(dataValues[4]));
  unsigned long curTime = millis();
  //180000 for 3 minutes
  if (curTime - prevTime > 15000) {
    Serial.println("SEND: " + String(dataValues[1]) + "\t" + String(dataValues[2]) + "\t" + String(dataValues[3]) + "\t" + String(dataValues[4]));
    rf95.send((uint8_t*) dataValues, sizeof(dataValues));
    rf95.waitPacketSent();
    count = 0;
    prevTime = curTime;
  }
}

