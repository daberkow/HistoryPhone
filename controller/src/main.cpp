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
#include "Mux.h"
using namespace admux;

Mux mux(Pin(13, OUTPUT, PinType::Digital), Pinset(32, 12, 27));
Mux inputsMux(Pin(35, INPUT, PinType::Digital), Pinset(5, 4, 0));

#define DIAL_1 19
#define DIAL_2_In_MOTION 21
// #define HOOK_SWITCH 21 // Hook switch moved to input mux channel 7
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
String filename = "";

Dialer dialer(DIAL_1, DIAL_2_In_MOTION);

int dialedNumber = 0;
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


String getYearsInJson(fs::FS &fs) {
    String returnData = "{\"folders\":[";
    File root = fs.open("/content"); // Open the directory
    bool firstFolder = true;

    while (true) {
        File folder = root.openNextFile();
        if (!folder) break; // No more files/folders

        if (folder.isDirectory()) {
            if (!firstFolder) {
                returnData += ",";
            }
            firstFolder = false;

            String folderName = folder.name();
            int photoCount = 0, textCount = 0, mp3Count = 0;

            File subFile;
            while ((subFile = folder.openNextFile())) {
                String fileName = subFile.name();
                if (fileName.endsWith(".jpg") || fileName.endsWith(".png")) {
                    photoCount++;
                } else if (fileName.endsWith(".txt")) {
                    textCount++;
                } else if (fileName.endsWith(".mp3")) {
                    mp3Count++;
                }
                subFile.close();
            }

            returnData += "{\"year\":\"" + folderName + "\",";
            returnData += "\"photos\":" + String(photoCount) + ",";
            returnData += "\"text_documents\":" + String(textCount) + ",";
            returnData += "\"mp3_files\":" + String(mp3Count) + "}";
        }
        folder.close();
    }
    root.close();

    returnData += "]}";
    return returnData;
}

int volumeLevel = 8;

/**
 * @brief Get the status of the audio playback for the web API.
 *
 * @return String JSON object containing playback status.
 */
String getPlaybackStatus() {
    String jsonResponse = "{";
    jsonResponse += "\"audio_file_playing\":\"" + filename + "\",";
    jsonResponse += "\"volume\":" + String(volumeLevel) + ",";
    jsonResponse += "\"total_time\":" + String(audio.getAudioFileDuration()) + ",";
    jsonResponse += "\"current_position\":" + String(audio.getAudioCurrentTime()) + ",";
    jsonResponse += "}";
    return jsonResponse;
}

String queued = "";

void setup() {
    Serial.begin(115200);
    Serial.println("History Phone Starting...");

    // Mux
    pinMode(32, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(DIAL_1, INPUT_PULLUP);
    pinMode(DIAL_2_In_MOTION, INPUT_PULLUP);
    // pinMode(HOOK_SWITCH, INPUT_PULLUP);
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

    // Debug: List web directory contents
    Serial.println("Checking /web directory:");
    File webDir = SD_MMC.open("/web");
    if (webDir && webDir.isDirectory()) {
        File file = webDir.openNextFile();
        while (file) {
            Serial.print("  - ");
            Serial.println(file.name());
            file = webDir.openNextFile();
        }
    } else {
        Serial.println("  /web directory not found!");
    }

    indexContentRoot(SD_MMC);

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(volumeLevel); // 0...21
    audio.forceMono(true);

    // Init Wifi
    // Load values saved in SPIFFS
    ssid = "HistoryPhone";
    pass = "";
    ip = "192.168.4.1";
    // gateway = "";
    Serial.println("SSID: " + ssid);
    Serial.println("Password: " + pass);
    Serial.println(ip);
    Serial.println(gateway);

    if (initWiFi()) {
        server.on("/api/current", HTTP_GET, [](AsyncWebServerRequest *request) {
            Serial.println("Current Year Requested");
            request->send(200, "text/plain", String(dialedNumber));
        });

        server.on("/api/queue", HTTP_POST, [](AsyncWebServerRequest *request) {
            Serial.println("Queue Pushed");
            if (request->hasParam("queue", true)) {
                const AsyncWebParameter* p = request->getParam("queue", true);
                queued = p->value();
                Serial.println("Received Queued data: " + queued);

                // Respond with a success message
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                // Respond with an error if the parameter is missing
                request->send(400, "application/json", "{\"error\":\"Missing 'volume' parameter\"}");
            }
        });

        server.on("/api/onhook", HTTP_GET, [](AsyncWebServerRequest *request) {
            Serial.println("Current onhook setting Requested");
            request->send(200, "text/plain", String(onHook));
        });

        server.on("/api/years", HTTP_GET, [](AsyncWebServerRequest *request) {
            Serial.println("Years Requested");
            String yearsJson = getYearsInJson(SD_MMC);
            request->send(200, "application/json", yearsJson);
        });

        server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
            Serial.println("Current status Requested");
            request->send(200, "text/plain", getPlaybackStatus());
        });

        server.on("/api/volume", HTTP_POST, [](AsyncWebServerRequest *request) {
            Serial.println("Volume Pushed");
            if (request->hasParam("volume", true)) { // Check if the POST body contains the "volume" parameter
                const AsyncWebParameter* p = request->getParam("volume", true); // Get the parameter
                String updatedVolume = p->value(); // Extract the value of the "volume" parameter
                Serial.println("Received volume data: " + updatedVolume);

                volumeLevel = updatedVolume.toInt();
                audio.setVolume(volumeLevel);

                // Respond with a success message
                request->send(200, "application/json", "{\"status\":\"success\"}");
            } else {
                // Respond with an error if the parameter is missing
                request->send(400, "application/json", "{\"error\":\"Missing 'volume' parameter\"}");
            }
        });

        // Route for root / web page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            Serial.println("Root path '/' requested");
            request->send(SD_MMC, "/web/index.html", "text/html");
        });

        // Serve content directory
        server.serveStatic("/content/", SD_MMC, "/content/").setCacheControl("public, max-age=86400");

        // Serve Next.js static export assets
        server.serveStatic("/", SD_MMC, "/web/").setCacheControl("public, max-age=86400");

        // Add a catch-all handler for debugging
        server.onNotFound([](AsyncWebServerRequest *request) {
            Serial.print("NOT FOUND: ");
            Serial.println(request->url());
            request->send(404, "text/plain", "Not found");
        });

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

// Hook switch debouncing state
boolean hookReadings[5] = {false, false, false, false, false};
int readingIndex = 0;
boolean readingsReady = false;

void loop() {
    // Take one reading per loop iteration to avoid blocking
    inputsMux.channel(7); // Hook switch on channel 7
    delayMicroseconds(10); // Allow MUX to settle
    hookReadings[readingIndex] = inputsMux.read();
    readingIndex++;

    // Once we have 5 readings, calculate the average
    if (readingIndex >= 5) {
        readingIndex = 0;
        readingsReady = true;
    }

    // Determine hook state from averaged readings
    int hookState = -1; // -1 means "not ready yet"
    if (readingsReady) {
        int total = 0;
        for (int i = 0; i < 5; i++) {
            total += hookReadings[i];
        }
        float average = total / 5.0;
        // Serial.println("Hook Switch Avg: " + String(average));
        hookState = (average > 0.5) ? HIGH : LOW; // Threshold at midpoint
        hookState = !hookState; // Invert if needed based on your hardware
    }

    delay(2);
    // Serial.println("Hook State: " + String(hookState));

    // If readings aren't ready yet, just run audio loop and return
    if (hookState == -1) {
        audio.loop();
        return;
    }

    // Handle off-hook transition (handset picked up)
    if (onHook && hookState == LOW) {
        Serial.println("Off Hook");
        mux.channel(2);
        mux.write(1);
        onHook = false;
        busy = false;
        playing = false;
        folderContent.clear();
        // audio.connecttoFS(SD_MMC, "/content/tone.mp3");
        dialedNumber = 0;
        dialer.clearFinalPulseCount();
        lastReadTime = millis();
    }
    // Handle on-hook transition (handset hung up)
    else if (!onHook && hookState != LOW) {
        Serial.println("On Hook");
        mux.channel(2);
        mux.write(0);
        onHook = true;
        busy = false;
        playing = false;
        dialedNumber = 0;
        audio.stopSong();
        filename = "";
    }

    // Off-hook state: handle dialing and tone
    if (!onHook && !busy && !playing) {
        if (queued != "") {
            // queued value will be something like 'mp3_1921_0'
            Serial.println("Processing queued value: " + queued);
            int firstUnderscore = queued.indexOf('_');
            int secondUnderscore = queued.indexOf('_', firstUnderscore + 1);
            if (firstUnderscore != -1 && secondUnderscore != -1) {
                String folderStr = queued.substring(firstUnderscore + 1, secondUnderscore);
                int folderNum = folderStr.toInt();
                String fileStr = queued.substring(secondUnderscore + 1);
                Serial.println("Playing queued folder: " + folderStr + ", file: " + fileStr);

                playing = true;
                String filePathString = "/content/" + folderStr + "/" + fileStr + ".mp3";
                Serial.println("Playing: " + filePathString);
                audio.connecttoFS(SD_MMC, filePathString.c_str());
                filename = filePathString;
            }
        }

        // Keep dial tone playing
        if (!audio.isRunning()) {
            audio.connecttoFS(SD_MMC, "/content/tone.mp3");
            filename = "/content/tone.mp3";
        }

        // Check for dial pulses
        dialer.loop();
        if (dialer.getFinalPulseCount() > 0) {
            Serial.println("Dial Reading: " + String(dialer.getFinalPulseCount()));
            dialedNumber = (dialedNumber * 10) + dialer.getFinalPulseCount();
            dialer.clearFinalPulseCount();
            lastReadTime = millis();

            // Check if we have a complete 4-digit number
            if (dialedNumber > 999) {
                int closest = findClosestFolder(dialedNumber);
                Serial.println("Closest: " + String(closest));

                if (closest != -1) {
                    // Valid number - play audio content
                    playing = true;
                    indexPlayingContentsMetadata(SD_MMC, closest);
                    String randomFile = selectRandomAudioFile();
                    String filePathString = "/content/" + String(closest) + "/" + randomFile;
                    Serial.println("Playing: " + filePathString);
                    audio.connecttoFS(SD_MMC, filePathString.c_str());
                    filename = filePathString;
                } else {
                    // Invalid number - play busy signal
                    busy = true;
                    audio.connecttoFS(SD_MMC, "/content/busy.mp3");
                    filename = "/content/busy.mp3";
                }
            }
        } else {
            // Timeout if no dialing activity for 5 seconds
            if (millis() - lastReadTime > 7500) {
                busy = true;
                audio.connecttoFS(SD_MMC, "/content/busy.mp3");
                filename = "/content/busy.mp3";
            }
        }
    }

    // Keep busy signal looping
    if (!onHook && busy && !audio.isRunning()) {
        audio.connecttoFS(SD_MMC, "/content/busy.mp3");
        filename = "/content/busy.mp3";
    }

    // After audio clip finishes, play off-hook tone
    if (!onHook && playing && !audio.isRunning()) {
        playing = false;
        busy = true;
        audio.connecttoFS(SD_MMC, "/content/off-hook.mp3");
        filename = "/content/off-hook.mp3";
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
