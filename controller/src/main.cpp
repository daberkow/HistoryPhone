#include <Arduino.h>
#include <vector>

// SD Card Libraries
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Initialize SPIFFS
#include "SPIFFS.h"

#include "Audio.h"
#include "SD_MMC.h"
#include "FS.h"

#include "Dialer.h"

// WIFI
#include <WiFi.h>
#include <ESPmDNS.h>
#include "ESPAsyncWebServer.h"

const int BUFFER_SIZE = 1024;

// **** IO Pins ****
// Digital I/O used
#define SD_MMC_CMD 15 //Please do not modify it.
#define SD_MMC_CLK 14 //Please do not modify it.
#define SD_MMC_D0  2  //Please do not modify it.

//External Dac
#define I2S_DOUT      25
#define I2S_BCLK      26
#define I2S_LRC       33

// IO
#define DIAL_1 21
#define DIAL_2_In_MOTION 19
#define HOOK_SWITCH 27
// **** IO Pins ****


// Wifi vars
String ssid;
String pass;
String ip;
String gateway;
IPAddress localIP;
// Set your Gateway IP address
IPAddress localGateway;
// IPAddress subnet(255, 255, 0, 0);
// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// https://registry.platformio.org/libraries/esphome/ESP32-audioI2S
Audio audio;

Dialer dialer(HOOK_SWITCH, DIAL_1, DIAL_2_In_MOTION);

int number = 0;
bool onHook = true;
bool busy = false;
bool playing = false;
unsigned long lastReadTime;

void initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
    Serial.println("SPIFFS mounted successfully");
}

/**
 * @brief Read File from SPIFFS
 */
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

/**
 * @brief Write a file to SPIFFS
 */
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

/**
 * @brief Init Wifi
 */
bool initWiFi() {
    if (ssid == "" || ip == "") {
        Serial.println("Undefined SSID or IP address.");
        return false;
    }

    WiFi.mode(WIFI_AP);
    // localIP.fromString(ip.c_str());
    // localGateway.fromString(gateway.c_str());

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

std::vector<int> content;
/**
 * @brief Index the content root, and store the folder numbers in a vector.
 */
void indexContentRoot(fs::FS &fs) {
    const char * dirname = "/content";
    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            content.push_back(atoi(file.name()));
        }
        file = root.openNextFile();
    }
}

std::vector<String> folderContent;
/**
 * @brief Index the contents of a year folder, and store the metadata in a
 * vector.
 */
void indexPlayingContentsMetadata(fs::FS &fs, int folderNum) {
    String folderPath = "/content/" + String(folderNum);
    const char * dirname = folderPath.c_str();
    Serial.println("Indexing: " + String(dirname));
    File root = fs.open(folderPath);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            Serial.print("  File : ");
            Serial.println(file.name());
            folderContent.push_back(file.name());
        }
        file = root.openNextFile();
    }
}

/**
 * @brief Select a random audio file from the folder of content that has been
 * indexed.
 */
String selectRandomAudioFile() {
    int mp3_files = 0;
    for (int i = 0; i < folderContent.size(); i++) {
        if (folderContent[i].endsWith(".mp3")) {
            mp3_files++;
        }
    }
    if (mp3_files == 0) {
        return "";
    }
    int randomIndex = random(0, mp3_files);
    for (int i = 0; i < folderContent.size(); i++) {
        if (folderContent[i].endsWith(".mp3")) {
            if (randomIndex == 0) {
                return folderContent[i];
            }
            randomIndex--;
        }
    }
    return "";
}

void setup() {
    Serial.begin(115200);
    pinMode(DIAL_1, INPUT_PULLUP);
    pinMode(DIAL_2_In_MOTION, INPUT_PULLUP);
    pinMode(HOOK_SWITCH, INPUT_PULLUP);
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);

    // https://randomnerdtutorials.com/getting-started-freenove-esp32-wrover-cam/
    // https://github.com/Freenove/Freenove_ESP32_WROVER_Board/blob/main/Datasheet/ESP32-Pinout.pdf

    // Init SD card
    // https://randomnerdtutorials.com/esp32-microsd-card-arduino/
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
        Serial.println("Card Mount Failed");
    }
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
    }
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC ){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    Serial.printf("Total space: %lluMB\n", SD_MMC.cardSize() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD_MMC.usedBytes() / (1024 * 1024));
    // listDir(SD_MMC, "/", 0);
    indexContentRoot(SD_MMC);

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(14); // 0...21
    audio.forceMono(true);

    // Init Wifi
    // Load values saved in SPIFFS
    ssid = "HistoryPhone";
    pass = "";
    ip = "192.168.4.1";
    // gateway = "";
    Serial.println("SSID: " + ssid);
    Serial.println("Password: " + pass);
    // Serial.println(ip);
    // Serial.println(gateway);

    if (initWiFi()) {
        server.on("/api/current", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "text/plain", String(number));
        });

        // Route for root / web page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SD_MMC, "/index.html", "text/html", false, processor);
        });
        server.serveStatic("/", SD_MMC, "/web/");
        server.begin();

        if (!MDNS.begin("phone")) {
            Serial.println("Error starting mDNS");
        }
    }
}

/**
 * @brief Find the closest folder to the number dialed. We use the index of the
 * content folder from boot, if a number is dialed and nothing is within 10,
 * then we play a busy signal.
 *
 * @param num The number dialed
 */
int findClosestFolder (int num) {
    int closest = -1;
    int diff = 1000;
    for (int i = 0; i < content.size(); i++) {
        int temp = abs(content[i] - num);
        if (temp > 10) {
            continue;
        }
        if (temp < diff) {
            diff = temp;
            closest = content[i];
        }
    }
    return closest;
}

void loop() {
    int hookState = digitalRead(HOOK_SWITCH);

    if (onHook && hookState == LOW) {
        Serial.println("Off Hook");
        // Set states when first taken off hook
        onHook = false;
        busy = false;
        playing = false;
        folderContent.clear();

        // Start playing tone, and blank counters
        audio.connecttoFS(SD_MMC, "/content/tone.mp3");
        number = 0;
        dialer.clearFinalPulseCount();
        lastReadTime = millis();
    } else if (!onHook && hookState != LOW) {
        Serial.println("On Hook");
        onHook = true;
        busy = false;
        playing = false;
        number = 0;
        audio.stopSong();
    }

    // We are off the hook, but not done playing busy signal
    if (!onHook && !busy && !playing) {
        // Currently off hook, do things related to that
        if (!audio.isRunning()) {
            audio.connecttoFS(SD_MMC, "/content/tone.mp3");
        }
        dialer.loop();
        if (dialer.getFinalPulseCount() > 0) {
            Serial.println("Dial Reading: " + String(dialer.getFinalPulseCount()));
            number = (number * 10) + dialer.getFinalPulseCount();
            dialer.clearFinalPulseCount();
            lastReadTime = millis();
            if (number > 999) {
                // We have a 4 digit number, and we have not timed out
                // PLAY AUDIO CLIP!
                int closest = findClosestFolder(number);
                Serial.println("Closest: " + String(closest));
                if (closest != -1) {
                    playing = true;
                    indexPlayingContentsMetadata(SD_MMC, closest);
                    String randomFile = selectRandomAudioFile();
                    String filePathString = ("/content/" + String(closest) + "/" + randomFile);
                    const char *filePath = filePathString.c_str();
                    Serial.println("Playing: " + filePathString);
                    audio.connecttoFS(SD_MMC, filePath);
                } else {
                    busy = true;
                    audio.connecttoFS(SD_MMC, "/content/busy.mp3");
                }
            }
        } else {
            if (millis() - lastReadTime > 5000) {
                busy = true;
                audio.connecttoFS(SD_MMC, "/content/busy.mp3");
            }
        }
    }
    if (!onHook && busy && !audio.isRunning()) {
        audio.connecttoFS(SD_MMC, "/content/busy.mp3");
    }

    if (!onHook && playing && !audio.isRunning()) {
        playing = false;
        busy = true;
        audio.connecttoFS(SD_MMC, "/content/off-hook.mp3");
    }
    audio.loop();
}

// optional
// void audio_info(const char *info){
//     Serial.print("info        "); Serial.println(info);
// }
// void audio_id3data(const char *info){  //id3 metadata
//     Serial.print("id3data     ");Serial.println(info);
// }
// void audio_eof_mp3(const char *info){  //end of file
//     Serial.print("eof_mp3     ");Serial.println(info);
// }
// void audio_showstation(const char *info){
//     Serial.print("station     ");Serial.println(info);
// }
// void audio_showstreamtitle(const char *info){
//     Serial.print("streamtitle ");Serial.println(info);
// }
// void audio_bitrate(const char *info){
//     Serial.print("bitrate     ");Serial.println(info);
// }
// void audio_commercial(const char *info){  //duration in sec
//     Serial.print("commercial  ");Serial.println(info);
// }
// void audio_icyurl(const char *info){  //homepage
//     Serial.print("icyurl      ");Serial.println(info);
// }
// void audio_lasthost(const char *info){  //stream URL played
//     Serial.print("lasthost    ");Serial.println(info);
// }
// void audio_eof_speech(const char *info){
//     Serial.print("eof_speech  ");Serial.println(info);
// }
