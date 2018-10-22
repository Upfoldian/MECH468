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
const int analogPIR = A0;


void loraInit() {
  rf95.init();
  rf95.setFrequency(frequency); // Setup ISM frequency
  rf95.setTxPower(13); // Setup Power,dBm
  rf95.setSpreadingFactor(7); // Setup Spreading Factor (6 ~ 12)
  rf95.setSignalBandwidth(125000);   // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  rf95.setCodingRate4(5);  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
}


void setup() {
  Serial.begin(9600);
  pinMode(digitalPIR, INPUT);
  pinMode(DHTpin, INPUT);
  loraInit();
}
unsigned long prevTime = millis();
bool PIRtoggle = false;
uint8_t count = 0;

void loop() {
  if (digitalRead(digitalPIR) == HIGH && PIRtoggle == false) {
    PIRtoggle = true;
    count++;
  } else if (digitalRead(digitalPIR) == LOW) {
    PIRtoggle = false;
  }
  
  DHT.read11(DHTpin);
  uint8_t dataValues[4];
  uint8_t temperature = (uint8_t)DHT.temperature;
  uint8_t humidity = (uint8_t)DHT.humidity;
  dataValues[0] = 55;
  dataValues[1] = temperature;
  dataValues[2] = humidity;
  dataValues[3] = count;
  //the DHT11 is only precise to integer values so I dunno why the library uses doubles. 
  //I might change this later to reduce the transmission data packet by 75%.
  //Serial.println(String(dataValues[1]) + "\t" + String(dataValues[2]) + "\t" + String(dataValues[3]));
  
  unsigned long curTime = millis();
  if (curTime - prevTime > 15000) {
    Serial.println("SEND: " + String(dataValues[1]) + "\t" + String(dataValues[2]) + "\t" + String(dataValues[3]));
    rf95.send((uint8_t*) dataValues, sizeof(dataValues));
    rf95.waitPacketSent();
    count = 0;
    prevTime = curTime;
  }
  delay(300);
}

