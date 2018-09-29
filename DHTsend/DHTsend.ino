//LoRa libs and variables
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;
float frequency = 868.0;

//Sensor libs and variables
// Documentation http://playground.arduino.cc/Main/DHTLib
#include <dht.h>

dht DHT;
const int DHTpin = 7;


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
  loraInit();
}

void loop() {
  DHT.read11(DHTpin);
  double dataValues[2];
  double temperature = DHT.temperature;
  double humidity = DHT.humidity;
  dataValues[0] = temperature;
  dataValues[1] = humidity;
  //the DHT11 is only precise to integer values so I dunno why the library uses doubles. I might change this later to reduce the transmission by 75%.
  Serial.println(String(temperature) + "\t" + String(humidity) + "\t" + String(sizeof(dataValues)));
  
  rf95.send((uint8_t*) dataValues, sizeof(dataValues));
  rf95.waitPacketSent();

  
  /* If I need to send back the data in future
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf95.waitAvailableTimeout(3000)) { 
    if (rf95.recv(buf, &len)) {
      Serial.print("reply: ");
      Serial.println((char*)buf);
    } else {
      Serial.println("recv failed");
    }
  } else {
    Serial.println("No reply, is LoRa server running?");
  }
  */
  delay(300);
}

