//LoRa libs and variables
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;
float frequency = 868.0;

//System call lib
#include <Process.h>
#include <Console.h>

void loraInit() {
  rf95.init();
  rf95.setFrequency(frequency); // Setup ISM frequency
  rf95.setTxPower(13); // Setup Power,dBm
  rf95.setSpreadingFactor(7); // Setup Spreading Factor (6 ~ 12)
  rf95.setSignalBandwidth(125000);   // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  rf95.setCodingRate4(5);  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8) 
}

void setup() {
  Bridge.begin(115200);
  Console.begin();
  loraInit();
}

void loop() {
  Process p;
  uint8_t buf[8];
  uint8_t len = sizeof(buf);
  if (rf95.waitAvailableTimeout(3000)) { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len) && buf[0] == 55) {
      // Field 1 = People
      // Field 2 = Temp
      // Field 3 = Humid
      // Field 8 = MotorPosition
      String request = "https://api.thingspeak.com/update?api_key=4ZSKIBA1TZHJPS3U&field2=" + String(buf[1]);
      request += "&field3=" + String(buf[2]);
      request +="&field1=" + String(buf[3]);
      request +="&field8=" + String(buf[4]);
      p.begin("curl");
      p.addParameter("-k");
      p.addParameter(request);
      p.run();
      delay(15000);  
    }
  }
}

