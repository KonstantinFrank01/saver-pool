#ifndef _POWERSWITCHSENSOR_h
#define _POWERSWITCHSENSOR_h

#include "arduino.h"
#include "Sensor.h"
#include <HLW8012.h>

#define POWER_MEASUREMENTS 5

/*
*/
class SonoffPowerMeter : public Sensor
{

public:
	SonoffPowerMeter(const char* nodeName, const char* name, const char* unit, float threashold);
	virtual void measure();

private:
	void calibrate();
	void setInterrupts();
	long _lastMeasurementMilliSecond;
	int _powerMeasurements[POWER_MEASUREMENTS];
	int _actIndex;

};
#endif

