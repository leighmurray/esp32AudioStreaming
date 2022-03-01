/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include "AudioManager.h"
#include "secrets.h"
#include "DisplayManager.h"

// WiFi network name and password:
const char * networkName = SECRET_SSID;
const char * networkPswd = SECRET_PASS;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;
constexpr uint16_t kAudioPort = 4464;
DisplayManager displayManager = DisplayManager();

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  Serial.println("Hello");
  displayManager.Setup();
  displayManager.setTextColor(TFT_WHITE, TFT_BLACK);
  
  displayManager.fillScreen(TFT_BLACK);            // Clear screen
  displayManager.setFreeFont(FF18);                 // Select the font
  displayManager.drawString(sFF1, 0, 0, GFXFF);// Print the string name of the font
  displayManager.setFreeFont(FF1);                 // Select the font
  displayManager.drawString("Connecting to wifi", 0, 30, GFXFF);// Print the string name of the font
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
  SetupAudio();

}


void loop(){
  //only send data when connected
  if(connected){
    handleRemoteAudio();
  }
  //Wait for 1 second
  //delay(1);
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case ARDUINO_EVENT_WIFI_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),kAudioPort);
          // Send a packet to the server to let them know we are here.
          udp.beginPacket("tas.leighmurray.com", kAudioPort);
          uint8_t emptyBuffer[512];
          udp.write(emptyBuffer, 512);
          //udp.printf("Seconds since boot: %lu", millis()/1000);
          udp.endPacket();
          connected = true;
          break;
      case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
      default: break;
    }
}

bool receiveAudioBuffers(byte outputAudioBufferLeft[256], byte outputAudioBufferRight[256]){
  uint8_t buf[528];
  uint16_t size = udp.parsePacket();
  if (0 < size && size <= sizeof(buf)) {
    udp.read(buf, size);
    //for (int i=0; i<size; i++) Serial.print(buf[i], HEX);
    //Serial.println();
    memcpy(outputAudioBufferLeft, &buf[16], 256);
    memcpy(outputAudioBufferRight, &buf[16+256], 256);
    return true;
  }
  return false;
}

void handleRemoteAudio(){
  byte outputAudioBufferLeft[256];
  byte outputAudioBufferRight[256];
  if (receiveAudioBuffers(outputAudioBufferLeft, outputAudioBufferRight)){
    PlayAudioBuffer(outputAudioBufferLeft, outputAudioBufferRight);
  }
}
