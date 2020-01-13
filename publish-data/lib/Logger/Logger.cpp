
#include "Logger.h"
#include <MqttClient.h>
#include <NodeTime.h>

/*
	Ein Logeintrag mit folgenden Parametern wird zum Server geschickt:
	topic: Topic, für das der LogEntry ausgelöst wurde
	type: Typ der Logmessage (Info - Fatalerror)
	time: Unix-Timecode
	message: Logtext
*/
void LoggerClass::log(const char* topic, int type, const char* message)
{
	Serial.print(F("*LG: "));
	Serial.print(topic);
	Serial.print(F(" => ["));
	Serial.print(type);
	Serial.print(F("] => "));
	Serial.println(message);
	char fullTopic[100] = "log/";
	strcat(fullTopic, topic);
	_jsonBuffer.clear();
	JsonObject& logEntry = _jsonBuffer.createObject();
	logEntry["type"] = type;
	logEntry["time"] = NodeTime.getDateTime();
	logEntry["message"] = message;
	char jsonText[128];
	logEntry.printTo(jsonText);
	Serial.print(F("*LG: Topic: "));
	Serial.print(fullTopic);
	Serial.print(", Payload: ");
	Serial.println(jsonText);
	MqttClient.publish(fullTopic, jsonText);
}


LoggerClass Logger;

