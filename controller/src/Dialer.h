#ifndef DIALER_H
#define DIALER_H

#include <Arduino.h>

class Dialer {
public:
    Dialer(int hookSwitchPin, int dial1Pin, int dial2InMotionPin);
    void loop();
    int getFinalPulseCount();
    void clearFinalPulseCount();

private:
    int hookSwitchPin;
    int dial1Pin;
    int dial2InMotionPin;
    boolean onHook;
    int pulseCount;
    boolean wasInMotion;
    boolean lastReadOpen;

    int finalPulseCount;
    unsigned long lastReadTime;
};

#endif // DIALER_H
