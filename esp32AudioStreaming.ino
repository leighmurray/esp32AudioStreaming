/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>
#include "AudioManager.h"
#include "secrets.h"

// WiFi network name and password:
const char * networkName = SECRET_SSID;
const char * networkPswd = SECRET_PASS;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;
constexpr uint16_t kAudioPort = 4464;

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  
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
  uint8_t buf[512];
  uint16_t size = udp.parsePacket();
  if (0 < size && size <= sizeof(buf)) {
    udp.read(buf, size);
    memcpy(outputAudioBufferLeft, buf, 256);
    memcpy(outputAudioBufferRight, &buf[256], 256);
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
