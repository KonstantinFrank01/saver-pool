#ifndef _SonoffSwitchSENSOR_h
#define _SonoffSwitchSENSOR_h

#include "arduino.h"
#include "Sensor.h"

/*
	Der SonoffSwitchSensor ist der zum Actor SonoffSwitch zugehörige Sensor.
	Damit wird der aktuelle Zustand des digitalen Schaltpins ausgelesen.
*/
class SonoffSwitchSensor : public Sensor
{

public:
	SonoffSwitchSensor(const char* nodeName, const char* name, const char* unit, float threashold, uint8_t pinNumber);
	virtual void measure();

private:
	int _pinNumber;

};
#endif

