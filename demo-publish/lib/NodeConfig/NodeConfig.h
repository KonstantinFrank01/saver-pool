// NodeConfig.h
#pragma once

#define MQTT_SERVERADDRESS_LENGTH 40
#define MQTT_PORT_LENGTH 6
#define MQTT_USERNAME_LENGTH 20
#define MQTT_PASSWORD_LENGTH 20
#define MQTT_NODE_NAME_LENGTH 80


class NodeConfigClass
{
private:
	// char _mqttServerAddress[MQTT_SERVERADDRESS_LENGTH]="10.0.0.235";
	char _mqttServerAddress[MQTT_SERVERADDRESS_LENGTH]="127.0.0.1";
	char _mqttPort[MQTT_PORT_LENGTH] = "1883";
	char _mqttUserName[MQTT_USERNAME_LENGTH];
	char _mqttPassword[MQTT_PASSWORD_LENGTH];
	char _mqttNodeName[MQTT_NODE_NAME_LENGTH]= "iot/demo";

protected:


public:
	/*
		Beim Start der App werden die Konfigurationsdaten aus dem Configfile
		in die fields eingelesen.
	*/
	void readConfig();
	void saveConfig();
	void getConfigText(char* configText);

	char* getMqttServerAddress();
	char* getMqttPort();
	char* getMqttUserName();
	char* getMqttPassword();
	char* getMqttNodeName();

	void setMqttServerAddress(const char* mqttServerAddress);
	void setMqttPort(const char* mqttPort);
	void setMqttUserName(const char* mqttUserName);
	void setMqttPassword(const char* mqttPassword);
	void setMqttNodeName(const char* _mqttNodeName);
};

extern NodeConfigClass NodeConfig;


