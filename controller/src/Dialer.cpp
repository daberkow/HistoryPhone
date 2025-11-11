#include "Dialer.h"

Dialer::Dialer(int dial1Pin, int dial2InMotionPin)
    : dial1Pin(dial1Pin), dial2InMotionPin(dial2InMotionPin),
      onHook(true), pulseCount(0), wasInMotion(false), lastReadOpen(false), finalPulseCount(0), lastReadTime(0) {}

/**
 * This function is called in the main loop of the program.
 */
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

/**
 * This function returns the final pulse count.
 */
int Dialer::getFinalPulseCount() {
    return finalPulseCount;
}

/**
 * This function clears the final pulse count.
 */
void Dialer::clearFinalPulseCount() {
    finalPulseCount = 0;
}
