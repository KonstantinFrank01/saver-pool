// Logger.h
#pragma once

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#define LOG_TYPE_SYSTEM 0
#define LOG_TYPE_INFO 1
#define LOG_TYPE_DEBUG 2
#define LOG_TYPE_EXCEPTION 3
#define LOG_TYPE_ERROR 4
#define LOG_TYPE_FATALERROR 5

class LoggerClass
{
public:
	/*
		Ein Logeintrag mit folgenden Parametern wird zum Server geschickt:
		topic: Topic, für das der LogEntry ausgelöst wurde
		type: Typ der Logmessage (Info - Fatalerror)
		time: Unix-Timecode
		message: Logtext
	*/
	void log(const char* topic, int type, const char* message);
private:
	StaticJsonBuffer<200> _jsonBuffer;
};
extern LoggerClass Logger;


