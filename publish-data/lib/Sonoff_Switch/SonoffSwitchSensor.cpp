#include <SonoffSwitchSensor.h>

SonoffSwitchSensor::SonoffSwitchSensor(const char *nodeName, const char *name, const char *unit, float threashold, uint8_t pinNumber)
	: Sensor(nodeName, name, unit, threashold)
{
	Serial.print(F("*PS: Sensor initialized: "));
	Serial.println(name);
	_pinNumber = pinNumber;
}

void SonoffSwitchSensor::measure()
{
	if (digitalRead(_pinNumber) == 0)
	{
		// Serial.println(F("*PS: SonoffSwitchSensor low ==> return 1"));
		setMeasurement(0);
	}
	else
	{
		setMeasurement(1);
		// Serial.println(F("*PS: SonoffSwitchSensor high ==> return 0"));
	}
}
