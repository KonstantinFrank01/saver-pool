#include "Sensor.h"
#include <NodeTime.h>
// #include <Node.h>
#include <MqttClient.h>

//#define Sensor_DEBUG


Sensor::Sensor(const char* nodeName, const char* name, const char* unit, float threshold) {
	strcpy(_nodeName, nodeName);
	strcpy(_name, name);
	strcpy(_unit, unit);
	_threshold = threshold;
	_measurement = 0;
	_lastMeasurement = 0;
	_time = 0;
	Serial.print(F("*SE: Sensor initialized: "));
	Serial.println(name);
}

/**
 * Der konkrete Sensor meldet einen Messwert. 
 * Es wird überprüft, ob der Messwert gemeldet werden muss (Zeitablauf, Änderung)
 * und bei Bedarf per MQTT gemeldet.
 */
void Sensor::setMeasurement(float value) {
	_lastMeasurement = value;
	long time = NodeTime.getDateTime();
	if (abs(value - _measurement) >= _threshold || time > _time + 900)
	{
		Serial.println(F("*SE: Sensor::setMeasurement()"));
		_measurement = value;
		_time = time;
		Serial.print(F("*SE: Neuer Messwert fuer "));
		Serial.print(_name);
		Serial.print(": ");
		Serial.print(_measurement, 1);
		Serial.print(" ");
		Serial.print(_unit);
		Serial.print(", Time: ");
		Serial.println(_time);
		char fullTopic[100];
		//!sprintf(fullTopic, "%s/%s", Node.getName(), _name);
		sprintf(fullTopic, "%s/%s", _nodeName, _name);
		_jsonBuffer.clear();
		JsonObject& payload = _jsonBuffer.createObject();
		payload["time"] = NodeTime.getDateTime();
		payload["value"] = value;
		char jsonText[128];
		payload.printTo(jsonText);
		Serial.print(F("*SE: Topic: "));
		Serial.print(fullTopic);
		Serial.print(", Payload: ");
		Serial.println(jsonText);
		MqttClient.publish(fullTopic, jsonText);
	}
}

float Sensor::getLastMeasurement() {
	return _lastMeasurement;
}

char* Sensor::getName() {
	return _name;
}

//void Sensor::measure(){}
