#include "Node.h"
#include <MqttClient.h>

void NodeClass::init(const char *name)
{
	strcpy(_nodeName, name);
	MqttClient.init(_nodeName);
}

char *NodeClass::getName()
{
	return _nodeName;
}

void NodeClass::addSensor(Sensor *sensorPtr)
{
	_sensors.push_back(sensorPtr);
}

void NodeClass::addActor(Actor *actorPtr)
{
	_actors.push_back(actorPtr);
}

void NodeClass::refreshSensorsAndActors()
{
	for (std::list<Sensor *>::iterator it = _sensors.begin(); it != _sensors.end(); ++it)
	{
		// Serial.print(F("*NO: refresh, Sensor: "));
		// Serial.print(it->first);
		// Serial.print(F(", last Value: "));
		// Serial.println(it->second->getLastMeasurement());
		Sensor *sensorPtr = *it;
		sensorPtr->measure();
	}
	for (std::list<Actor *>::iterator it = _actors.begin(); it != _actors.end(); ++it)
	{
		// Serial.print(F("*NO: refresh, Actor: "));
		// Serial.print(it->first);
		// Serial.print(F(", last Value: "));
		// Serial.println(it->second->getLastMeasurement());
		(*it)->syncValue();
	}
}

Actor *NodeClass::getActorByName(char *name)
{
	for (std::list<Actor *>::iterator it = _actors.begin(); it != _actors.end(); ++it)
	{
		// Serial.print(F("*NO: refresh, Sensor: "));
		// Serial.print(it->first);
		// Serial.print(F(", last Value: "));
		// Serial.println(it->second->getLastMeasurement());
		Actor *actorPtr = *it;
		if (strcmp(actorPtr->getName(), name) == 0)
		{
			return actorPtr;
		}
	}
	return nullptr;
}

NodeClass Node;
