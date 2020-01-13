// MqttClient.h
#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <list>

#define TOPIC_LENGTH 100
#define PAYLOAD_LENGTH 100

/*
	Definition des Typs des Functionpointers als Callback
	//! Sollte auch Methoden einer Klasse unterstützen (Readme.md updaten)
*/
typedef void(*SubscriberCallback)(const char *topic, const char *payload);

/*
	Zentrale Datenstruktur bei der Kommandobearbeitung
*/
struct MqttSubscription
{
	char *topic;
	SubscriberCallback subscriberCallback;
};

class MqttClientClass
{
  public:
	void init(const char *mainTopic);
	void addSubscription(MqttSubscription* subscriptionPtr);
	void publish(char *topic, char *payLoad);
	void doLoop();
	void notifySubscribers(char* topic, char *payload);
	boolean subscribeToBroker();
  private:
	char _mainTopic[50];
	std::list<MqttSubscription *> _mqttSubscriptions;

	//MqttCallback*  _subscriberCallback;

	WiFiClient _wifiClient;
	boolean reconnect();
	long _lastReconnectAttempt = 0;
	PubSubClient _mqttClient;
	char _topicCharArray[TOPIC_LENGTH];
	char _payloadCharArray[PAYLOAD_LENGTH];
};
extern MqttClientClass MqttClient;
