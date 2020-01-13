//InternLed.cpp
#include "InternLed.h"  // "" für gleiches Verzeichnis

#define FAST_PERIOD_MS  150
#define SLOW_PERIOD_MS  750

//#define ACTIVATE_DEBUG

#ifdef ACTIVATE_DEBUG
#  define D(x) x
#else
#  define D(x)
#endif // ACTIVATE_DEBUG

InternLedClass::InternLedClass() { _ticker = new Ticker();}

Ticker* InternLedClass::getTicker(){ return _ticker;}
uint32_t InternLedClass::getPeriodMs(){return _periodMs;}
uint8_t InternLedClass::getTimesToBlink(){return _timesToBlink;}
void InternLedClass::setTimesToBlink(uint8_t times){_timesToBlink=times;}

void InternLedClass::init() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void flip() {
    int state = digitalRead(LED_BUILTIN);  // get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);     // set pin to the opposite state
}

void InternLedClass::blinkFast() {
    init();
    _ticker->attach_ms(FAST_PERIOD_MS,flip);
}

void InternLedClass::blinkOff() {
    _ticker->detach();
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(LED_BUILTIN, INPUT);
}

void InternLedClass::blinkSlow() {
    init();
    _ticker->attach_ms(SLOW_PERIOD_MS,flip);
}

/*
    Led-Zustand invertieren und sich selbst als Callback mit der gewünschten
    Zeit registrieren, wenn die Anzahl noch nicht erreicht wurde.
*/
void flipSingle() {
    int state = digitalRead(LED_BUILTIN);       // get the current state of GPIO1 pin
    digitalWrite(LED_BUILTIN, !state);          // set pin to the opposite state
    uint32_t period = InternLed.getPeriodMs();  
    uint8_t times = InternLed.getTimesToBlink()-1;
    D(                                  
     Serial.print("Times: ");
     Serial.println(times);
    )
    if(state){
        InternLed.setTimesToBlink(times);
    }
    if(times > 0){
          InternLed.getTicker()->once_ms(period,flipSingle);  // noch einmal starten
    }
}

void InternLedClass::blinkFast(uint8_t times) {
    init();
    _timesToBlink = times; // HIGH und LOW
    _periodMs = FAST_PERIOD_MS;
    _ticker->attach_ms(FAST_PERIOD_MS,flipSingle);
}

void InternLedClass::blinkSlow(uint8_t times) {
    init();
    _timesToBlink = times;
    _periodMs = SLOW_PERIOD_MS;
    _ticker->attach_ms(SLOW_PERIOD_MS,flipSingle);
}

// Quasi Singleton
InternLedClass InternLed;