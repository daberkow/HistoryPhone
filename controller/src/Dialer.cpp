#include "Dialer.h"

Dialer::Dialer(int hookSwitchPin, int dial1Pin, int dial2InMotionPin)
    : hookSwitchPin(hookSwitchPin), dial1Pin(dial1Pin), dial2InMotionPin(dial2InMotionPin),
      onHook(true), pulseCount(0), wasInMotion(false), lastReadOpen(false), finalPulseCount(0), lastReadTime(0) {}

void Dialer::loop() {
    // this is reading too fast, I want to add a 10ms delay that doesnt block the rest of the system, just skips if its been less than 10ms
    if (millis() - lastReadTime < 10) {
        return;
    } else {
        lastReadTime = millis();
    }

    int readingPulse = digitalRead(dial1Pin);
    boolean inMotion = (digitalRead(dial2InMotionPin) == LOW);

    if (inMotion && !wasInMotion) {
        pulseCount = 0;
        wasInMotion = true;
    } else {
        if (!inMotion && wasInMotion) {
            // Serial.println("Pulse Count: " + String(pulseCount));
            finalPulseCount = pulseCount;
            wasInMotion = false;
        }
    }
    if (inMotion) {
        if (readingPulse == HIGH && lastReadOpen == false) {
            // Serial.println("Pulse");
            pulseCount++;
            lastReadOpen = true;
        } else if (readingPulse == LOW && lastReadOpen == true) {
            lastReadOpen = false;
        }
    }
}

int Dialer::getFinalPulseCount() {
    // int tempPulse = finalPulseCount;
    // finalPulseCount = 0;
    return finalPulseCount;
}

void Dialer::clearFinalPulseCount() {
    finalPulseCount = 0;
}
