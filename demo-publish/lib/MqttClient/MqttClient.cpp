#include "MqttClient.h"
#include <NodeConfig.h>

/*
* Zentrale Callbackroutine, die alle Nachrichten der registrierten
* Subscriber empfängt und dann an die Callbackmethode des 
* Subscribers weiterleitet
* Reine C-Funktion ohne direkten Zugriff auf fields und methods
* Kommunikation über Singleton.
*/
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  char payloadText[100];
  for (int i = 0; i < 100; i++)
  {
    payloadText[i] = 0;
  }
  int i;
  for (i = 0; i < length; i++)
  {
    payloadText[i] = (char)payload[i];
  }
  payloadText[i] = 0;
  Serial.print(F("*MC: Message arrived ["));
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(payloadText);
  MqttClient.notifySubscribers(topic, payloadText);
}

/**
 * Beginnt der Text mit den Zeichen des Patterns?
 */
bool strStartsWith(char *text, char *pattern)
{
  int length = strlen(pattern);
  if (strlen(text) < length)
  {
    return false;
  }
  for (int i = 0; i < length; i++)
  {
    if (text[i] != pattern[i])
      return false;
  }
  return true;
}

/**
 * Überprüft, welche Subscriber das Topic registriert haben und 
 * verständigt die entsprechenden Subscriber.
 * //! Derzeit wird nur überprüft ob das empfangene Topic mit dem registrierten
 *      Text beginnt. Später auch um Wildcards erweitern (Readme.md auch updaten).
 */
void MqttClientClass::notifySubscribers(char *topic, char *payload)
{
  for (std::list<MqttSubscription *>::iterator it = _mqttSubscriptions.begin();
       it != _mqttSubscriptions.end(); ++it)
  {
    MqttSubscription *subscriptionPtr = *it;
    if (strStartsWith(topic, subscriptionPtr->topic))
    {
      subscriptionPtr->subscriberCallback(topic, payload);
    }
  }
}

// MqttCallback* MqttClientClass::getSubscriberCallback(){
//   return _subscriberCallback;
// }

void MqttClientClass::addSubscription(MqttSubscription *subscriptionPtr)
{
  _mqttSubscriptions.push_back(subscriptionPtr);
}

/**
 * Verbindung mit dem MQTT-Broker initialisieren
 */
void MqttClientClass::init(const char *mainTopic)
{
  strcpy(_mainTopic, mainTopic);
  Serial.print(F("*MC: MQTT-Broker Address: "));
  Serial.print(NodeConfig.getMqttServerAddress());
  int port = atoi(NodeConfig.getMqttPort());
  Serial.print(F(":"));
  Serial.println(port);
  _mqttClient.setClient(_wifiClient); // braucht er statisch
  _mqttClient.setServer(NodeConfig.getMqttServerAddress(), port);
  _mqttClient.setCallback(mqttCallback);
  Serial.println(F("*MC: MqttClient started"));
  reconnect();
}

/**
 * Alle registrierten Subscriptions müssen beim reconnect()
 * neu angemeldet werden.
 */
boolean MqttClientClass::subscribeToBroker()
{
  if (_mqttClient.connected())
  {
    Serial.println(F("*MC: subscribe to broker"));
    // Alle subscriptions wieder anmelden
    for (std::list<MqttSubscription *>::iterator it = _mqttSubscriptions.begin();
         it != _mqttSubscriptions.end(); ++it)
    {
      MqttSubscription *subscriptionPtr = *it;
      _mqttClient.subscribe(subscriptionPtr->topic);
      Serial.print(F("*MC: Subscribe for Topic: "));
      Serial.println(subscriptionPtr->topic);
    }
  }
}

/**
 * Versuchen, die Verbindung zum MQTT-Broker wieder
 * aufzubauen. Subscriber wieder anmelden
 */
boolean MqttClientClass::reconnect()
{
  Serial.println(F("*MC: reconnect MQTT"));
  if (_mqttClient.connect(NodeConfig.getMqttNodeName()))
  {
    Serial.println(F("*MC: Publish  reconnect"));
  }
  if (_mqttClient.connected())
  {
    Serial.println(F("*MC: MqttClient is connected"));
    subscribeToBroker();
  }
  else
  {
    Serial.println(F("!MC: MqttClient IS NOT connected"));
  }
  return _mqttClient.connected();
}

/**
 * Für das topic wird die payload an den Broker übertragen
 */
void MqttClientClass::publish(char *topic, char *payload)
{
  if (!_mqttClient.connected())
  {
    Serial.println(F("!MC: publish(), mqtt client not connected"));
  }
  else
  {
    Serial.print(F("*MC: Topic: "));
    Serial.print(topic);
    Serial.print(F(", Payload: "));
    Serial.print(payload);
    Serial.println(F(" published"));
    // publish with retained-flag
    _mqttClient.publish(topic, payload, true);
  }
}

/**
 * MqttClient-Verbindung aufrecht erhalten.
 * Wurde die Verbindung unterbrochen, reconnect versuchen
 */
void MqttClientClass::doLoop()
{
  // if connection lost ==> try to reconnect
  if (!_mqttClient.connected())
  {
    if (millis() - _lastReconnectAttempt > 5000)
    {
      Serial.println(F("!MC: doLoop(): MQTT Client not connected"));
      _lastReconnectAttempt = millis();
      if (reconnect())
      {
        _lastReconnectAttempt = 0;
      }
    }
  }
  else
  {
    // Serial.println("MQTT Client doLoop()");
    _mqttClient.loop();
  }
}

// Quasi Singletonimplementierung
MqttClientClass MqttClient;
