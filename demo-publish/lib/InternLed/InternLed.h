#pragma once

#include <Arduino.h>
#include <Ticker.h>

/* Die interne LED am ESP8266 kann für verschiedene Zwecke angesprochen werden
    kurzes Blinken mit oder ohne vorgegebener Anzahl
    langes Blinken mit oder ohne vorgegebener Anzahl
*/

class InternLedClass {
public:
    InternLedClass();
    void blinkFast();
    void blinkSlow();
    void blinkFast(uint8_t times);
    void blinkSlow(uint8_t times);
    void blinkOff();
    // Methoden für Zugriff auf Fields mittels
    // Callbackroutine (normale C-Funktion)
    uint32_t getPeriodMs();
    uint8_t getTimesToBlink();
    void setTimesToBlink(uint8_t times);
    Ticker* getTicker();
private: 
    void init();
    Ticker* _ticker;
    uint8_t _timesToBlink;
    uint32_t _periodMs;
};
extern InternLedClass InternLed;  










