#include <Logger.h>
#include <SonoffSwitch.h>
#include <SonoffSwitchSensor.h>
#include <Node.h>

/*
	Der Schalter hat keine Einheit des Messwertes EIN/AUS und auch eine fixe Schwelle von 0.001,
	da sowieso nur die Werte 0 und 1 verwendet werden.
*/
SonoffSwitch::SonoffSwitch(const char *nodeName, const char *name, int pinNumber) : Actor(nodeName, name)
{
	_sensorPtr = new SonoffSwitchSensor(nodeName, name, "", 0.001, pinNumber);
	_pinNumber = pinNumber;
	_settedValue = 0;
	pinMode(_pinNumber, OUTPUT);
	digitalWrite(_pinNumber, 1); // inverse Logik
}

void SonoffSwitch::changeActorTo(float value)
{
	if (value > 0.1)
	{
		Serial.println(F("*PS: set pin high!"));
		digitalWrite(_pinNumber, 1);
	}
	else
	{
		Serial.println(F("*PS: set pin low!"));
		digitalWrite(_pinNumber, 0);
	}
}
