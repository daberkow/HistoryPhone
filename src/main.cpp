#include <Arduino.h>

// SD Card Libraries
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Initialize SPIFFS
#include "SPIFFS.h"

// #define MINIMP3_IMPLEMENTATION
// #define MINIMP3_ONLY_MP3
// #define MINIMP3_NO_STDIO
// #include "minimp3.h"

// #include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"

// WIFI
#include <WiFi.h>

const int BUFFER_SIZE = 1024;
// Digital I/O used
#define SD_CS          4
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18

//External Dac
#define I2S_DOUT      0
#define I2S_BCLK      2
#define I2S_LRC       15

// Built In Dac
// #define I2S_DOUT      25
// #define I2S_BCLK      27
// #define I2S_LRC       26

#include "ESPAsyncWebServer.h"
// Wifi vars
String ssid;
String pass;
String ip;
String gateway;
IPAddress localIP;
// IPAddress localIP(192, 168, 1, 200); // hardcoded
// Set your Gateway IP address
IPAddress localGateway;
// IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);
// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// https://registry.platformio.org/libraries/esphome/ESP32-audioI2S
Audio audio;

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

// Initialize WiFi
bool initWiFi() {
  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_AP);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());

  WiFi.softAP(ssid, pass);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  return true;
}

// Replaces placeholder with LED state value
String processor(const String& var) {
  // if(var == "STATE") {
  //   if(digitalRead(ledPin)) {
  //     ledState = "ON";
  //   }
  //   else {
  //     ledState = "OFF";
  //   }
  //   return ledState;
  // }
  return String();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Init");
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  // https://randomnerdtutorials.com/getting-started-freenove-esp32-wrover-cam/
  // https://github.com/Freenove/Freenove_ESP32_WROVER_Board/blob/main/Datasheet/ESP32-Pinout.pdf

  // Init SD card
  // https://randomnerdtutorials.com/esp32-microsd-card-arduino/
  if(!SD.begin(SD_CS)){
    Serial.println("Card Mount Failed");
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // listDir(SD, "/", 0);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  // audio.setTone(-40, -40, -40);
  audio.setVolume(10); // 0...21
  audio.connecttoFS(SD, "/all.mp3");

  // Init Wifi
  // Load values saved in SPIFFS
  ssid = "DanTest";
  pass = "";
  ip = "172.168.0.1";
  gateway = "";
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);
  Serial.println(gateway);

  if(initWiFi()) {
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SD, "/index.html", "text/html", false, processor);
    });
    server.serveStatic("/", SD, "/");

    // // Route to set GPIO state to HIGH
    // server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    //   // digitalWrite(ledPin, HIGH);
    //   request->send(SD, "/index.html", "text/html", false, processor);
    // });

    // // Route to set GPIO state to LOW
    // server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    //   // digitalWrite(ledPin, LOW);
    //   request->send(SD, "/index.html", "text/html", false, processor);
    // });
    server.begin();
  }
}

void loop() {
  audio.loop();
}


// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}