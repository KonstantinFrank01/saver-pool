#include "Actor.h"
#include <NodeTime.h>
#include <Node.h>
#include <MqttClient.h>

//#define Actor_DEBUG

static void actorMqttCallback(const char *topic, const char *payload)
{
	// Aktor aus topic extrahieren
	int nodeNameLength = strlen(Node.getName());
	int topicLength = strlen(topic);
	char actorName[50];
	strncpy(actorName, topic + nodeNameLength + 1, topicLength - nodeNameLength);
	Serial.print(F("*AC: Actorname: "));
	Serial.println(actorName);
	Actor *actorPtr = Node.getActorByName(actorName);
	actorPtr->setSettedValueAsJson(payload);
}

/**
 * Aus der empfangenen Mqtt-Message ist der zu setzende
 * Wert des Aktors zu übernehmen und im Aktor zu setzen.
 * Beim nächsten Synchronisationsvorgang wird der
 * Aktor dann entsprechend angesteuert.
 */
void Actor::setSettedValueAsJson(const char *payload)
{
	_jsonBuffer.clear();
	JsonObject &jsonObject = _jsonBuffer.parseObject(payload);
	if (jsonObject.success())
	{
		if (jsonObject.containsKey("settedValue"))
		{
			_settedValue = jsonObject["settedValue"];
			Serial.print(F("*AC: Settedvalue: "));
			Serial.println(_settedValue);
		}
	}
	else
	{
		Serial.print(F("!AC: Payload isn't a valid json-object: '"));
		Serial.print(payload);
		Serial.println("'");
	}
}

Actor::Actor(const char *nodeName, const char *name)
{
	strcpy(_nodeName, nodeName);
	strcpy(_name, name);
	_time = NodeTime.getDateTime();
	strcpy(_actorTopic, Node.getName());
	strcat(_actorTopic, "/");
	strcat(_actorTopic, getName());
	_actorSubscription.topic = _actorTopic;
	_actorSubscription.subscriberCallback = actorMqttCallback;
	MqttClient.addSubscription(&_actorSubscription);
	_isStart = true;
}

void Actor::setSettedValue(float value)
{
	_settedValue = value;
}

float Actor::getActualValue()
{
	return _sensorPtr->getLastMeasurement();
}

float Actor::getSettedValue()
{
	return _settedValue;
}

char *Actor::getName()
{
	return _name;
}

Sensor *Actor::getSensor()
{
	return _sensorPtr;
}

void Actor::syncValue()
{
	_sensorPtr->measure();
	// Serial.print(F("*AC: actual value: "));
	// Serial.print(_sensorPtr->getLastMeasurement());
	// Serial.print(", settedValue: ");
	// Serial.println(_settedValue);
	float value = _sensorPtr->getLastMeasurement();
	if (abs(value - _settedValue) > 0.01 || _isStart)
	{
		_isStart = false;
		changeActorTo(_settedValue);
		char fullTopic[100];
		sprintf(fullTopic, "%s/%s", Node.getName(), _name);
		_jsonBuffer.clear();
		JsonObject &payload = _jsonBuffer.createObject();
		payload["time"] = NodeTime.getDateTime();
		payload["settedValue"] = _settedValue;
		payload["value"] = value;
		char jsonText[128];
		payload.printTo(jsonText);
		Serial.print(F("*AC: Topic: "));
		Serial.print(fullTopic);
		Serial.print(", Payload: ");
		Serial.println(jsonText);
		MqttClient.publish(fullTopic, jsonText);
	}
}

//void Actor::changeActorTo(float value) {}
