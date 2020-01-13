#include "FS.h"
#include "NodeConfig.h"
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

/*
	Verwaltung der Konfigurationsdaten der App in einer Datei (config.txt) im
	SPI-FlashFileSystem (SPIFFS).
	Die Konfiguration ist als JSON-Objekt gespeichert.
*/

/**
 * Konfiguration aus Datei in JSON-Objekt lesen und in Properties abspeichern
 */
void NodeConfigClass::readConfig(){
	if (SPIFFS.begin()) {
    	Serial.println(F("*NC: readConfig(), mounted file system"));
		if (SPIFFS.exists("/config.json")) {
			//file exists, reading and loading
			Serial.println(F("*NC: reading config file"));
			File configFile = SPIFFS.open("/config.json", "r");
			if (configFile) {
				Serial.println(F("*NC: opened config file"));
				size_t size = configFile.size();
				// Allocate a buffer to store contents of the file.
				std::unique_ptr<char[]> buf(new char[size]);

				configFile.readBytes(buf.get(), size);
				DynamicJsonBuffer configJsonBuffer;
				JsonObject& json = configJsonBuffer.parseObject(buf.get());
				Serial.print(F("*NC: Read Config:"));
				json.printTo(Serial);
				Serial.println();
				if (json.success()) {	
					Serial.println(F("*NC: json successfully read"));
					strcpy(_mqttServerAddress, json["mqtt_server"]);
					strcpy(_mqttPort, json["mqtt_port"]);
					strcpy(_mqttUserName, json["mqtt_user"]);
					strcpy(_mqttPassword, json["mqtt_pass"]);
					strcpy(_mqttNodeName, json["mqtt_node_name"]);
				} else {
					Serial.println(F("*NC: readConfig(), failed to parse json"));
				}	
			} else {
				Serial.println(F("*NC: readConfig(), failed to load json config"));
			}		
		} else{
			Serial.println(F("*NC: readConfig(), failed to open json config"));
		}
    } else {
    	Serial.println(F("*NC: failed to mount FS"));
  	}
}

/**
 * Konfiguration in JSON-Objekt umwandeln und in Datei schreiben
 */
void NodeConfigClass::saveConfig(){
	if (SPIFFS.begin()) {
		Serial.println(F("*NC: saving config"));
		DynamicJsonBuffer jsonBuffer;
		JsonObject& json = jsonBuffer.createObject();
		json["mqtt_server"] = _mqttServerAddress;
		json["mqtt_port"]   = _mqttPort;
		json["mqtt_user"]   = _mqttUserName;
		json["mqtt_pass"]   = _mqttPassword;
		json["mqtt_node_name"]  = _mqttNodeName;
		File configFile = SPIFFS.open("/config.json", "w");
		if (!configFile) {
			Serial.println(F("*NC: failed to open config file for writing"));
		}
		json.printTo(configFile);
		configFile.close();
		Serial.print(F("*NC: Saved Config:"));
		json.printTo(Serial);
		Serial.println();
	} else {
		Serial.println(F("*NC: failed to mount FS"));
	}
}

void NodeConfigClass::getConfigText(char* configText){
	sprintf(configText, "mqtt_node_name:%s, mqtt_server:%s, mqtt_port:%s, mqtt_user:%s", _mqttNodeName, _mqttServerAddress, _mqttPort, _mqttUserName);
}

char* NodeConfigClass::getMqttServerAddress() { return _mqttServerAddress; }
char* NodeConfigClass::getMqttPort() { return _mqttPort; }
char* NodeConfigClass::getMqttUserName() { return _mqttUserName; }
char* NodeConfigClass::getMqttPassword() { return _mqttPassword; }
char* NodeConfigClass::getMqttNodeName() { return _mqttNodeName; }

void NodeConfigClass::setMqttServerAddress(const char* mqttServerAddress) { strcpy(_mqttServerAddress, mqttServerAddress);}
void NodeConfigClass::setMqttPort(const char* mqttPort)  { strcpy(_mqttPort, mqttPort);}
void NodeConfigClass::setMqttUserName(const char* mqttUserName)  { strcpy(_mqttUserName, mqttUserName);}
void NodeConfigClass::setMqttPassword(const char* mqttPassword)  { strcpy(_mqttPassword, mqttPassword);}
void NodeConfigClass::setMqttNodeName(const char* mqttTopic)  { strcpy(_mqttNodeName, mqttTopic);}

NodeConfigClass NodeConfig;

