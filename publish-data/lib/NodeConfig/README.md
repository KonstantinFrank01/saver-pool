# Library NodeConfig

## Ziele

Diese Library implementiert die Verwaltung der Konfigurationsdaten des Nodes. Dazu gehören der Nodename und die IP-Adresse samt Portnummer des Servers.
Die Konfiguration wird im SPIFFS persistent gespeichert. Gibt es kein Config-File oder sind die Zugangsdaten ungültig (keine Anmeldung am WLAN möglich) eröffnet der ESP einen eigenen Accesspoint und wartet auf die Konfiguration per Webrequest.

## Hardware

Es wird lediglich ein NodeMCU benötigt.

## Software

### Testanwendung am NodeMCU

Das Testprogramm versucht sich bei Programmstart mit dem WLAN zu verbinden. Gelingt dies nicht oder gibt es noch keine persistierte Konfiguration wird ein Accesspoint eröffnet, dessen Name mit ESP beginnt und Teile der Mac-Adresse des Nodes enthält.

In der Loop wartet der ESP in beiden Fällen auf einen Webrequest, der die Konfiguration verändert. Am eigenen Accesspoint ist die Adresse 192.18.4.1, sonst die vom Router zugewiesene Adresse

Zur Verarbeitung des Webrequests wird der CommandInerpreter verwendet, dem die Callbackroutine "injiziert" wird.

````
	SimpleCommandInterpreter.init();
	NodeConfig.registerNodeConfigCommandCallback();
````

Der Request ist aus nachfolgenden Beispiel erkennbar:

````
ESP mit eigenem AP:
- http://192.168.4.1/config/ssid=SSID;password=PASSWORD;server=192.168.43.107;port=5679;name=NodeName

ESP im WLAN:
- http://192.168.0.13/?command=config&server=192.168.43.107&port=5679&name=NodeName
````

````c
#include <RtcUtility.h>
#include <RtcTemperature.h>
#include <RtcDS3231.h>
#include <RtcDS1307.h>
#include <RtcDateTime.h>
#include <LinkedList.h>
#include <WiFiUdp.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>
#include <Esp.h>

#include <HttpServer.h>
#include <WifiStation.h>
#include <SimpleCommandInterpreter.h>
#include "NodeConfig.h"


void setup() {
	Serial.begin(115200);                 //Initialisierung der seriellen Schnittstelle
	Serial.println();
	Serial.println();
	Serial.println("NodeConfigTest");
	Serial.println("==============");
	Serial.println();
	NodeConfig.setupWifi();
	delay(100);
	HttpServer.init();
	Serial.println("CommandInterpreter initialisieren");
	SimpleCommandInterpreter.init();
	NodeConfig.registerNodeConfigCommandCallback();
}


void loop() {
	yield();
	SimpleCommandInterpreter.interpretHttpCommand();
	delay(1);
}

````

#### Testanwendung verwenden

- Config noch nicht persistiert
	- PC mit AP des NodeMCU verbinden
		- http://192.168.4.1/config/ssid=SSID;password=PASSWORD;server=192.168.43.107;port=5679;name=NodeName
	- Auf Bestätigung "Config saved" warten
	- Node resetten und im seriellen Monitor kontrollieren, ob Verbindung funktioniert
- Config ist persistiert und funktioniert
	- Mit Node über "richtige" IP verbinden (serieller Output nach Reset)
	- http://192.168.0.13/config/ssid=XXXX;password=XXXX;server=192.168.43.107;port=5679;name=NodeName
	- Verbindung funktioniert nicht, Node eröffnet eigenen AP
	- Konfiguration wie oben


##### UI der Testanwendung

Korrekte Konfigurationsdaten:

````
NodeConfigTest
==============

Read Config from SPIFFS
readConfigFile() start
Config::init(), configText: ssid=XXXXXXXXX;password=XXXXXXXX;server=10.0.0.107;port=5679;name=IndoorRoot
setConfigToMap(), Key: ssid, Value: XXXXXXXX set to ConfigMap
setConfigToMap(), Key: password, Value: XXXXXXX set to ConfigMap
setConfigToMap(), Key: server, Value: 10.0.0.107 set to ConfigMap
setConfigToMap(), Key: port, Value: 5679 set to ConfigMap
setConfigToMap(), Key: name, Value: IndoorRoot set to ConfigMap
Connect to Wifi ......
WiFi connected, address: 10.0.0.222
Wifi is connected
Simple Webserver started, Status: 1
CommandInterpreter initialisieren
CommandInterpreterClass::init()
````

Fehlende oder fehlerhafte Konfigurationsdaten

````

NodeConfigTest
==============

Read Config from SPIFFS
readConfigFile() start
Config::init(), configText: ssid=XXX;password=XXX;server=192.168.43.107;port=5679;name=IndoorRoot
setConfigToMap(), Key: ssid, Value: XXX set to ConfigMap
setConfigToMap(), Key: password, Value: XXX set to ConfigMap
setConfigToMap(), Key: server, Value: 192.168.43.107 set to ConfigMap
setConfigToMap(), Key: port, Value: 5679 set to ConfigMap
setConfigToMap(), Key: name, Value: IndoorRoot set to ConfigMap
........................................
!!! setup(), Keine Verbindung zum WLAN!
Eigenen Accesspoint oeffnen: 
Setup AP: ESP_A220A6038F32
AP: ESP_A220A6038F32
IP: 192.168.4.1
Esp AP waiting on 192.168.4.1/config/ssid=SSID_WLAN;password=PASSWORD_WLAN;server=10.0.0.52;port=5679;name=cistern
Simple Webserver started, Status: 1
Client connected, IP: 192.168.4.2
Webrequest: config/ssid=linksysAtHome;password=YJXPF4WAC7;server=192.168.0.3;port=5679;name=Wemos
ConfigString: ssid=linksysAtHome;password=YJXPF4WAC7;server=192.168.0.3;port=5679;name=Wemos
setConfigByConfigString(), Request: ssid=____;password=____;server=192.168.0.3;port=5679;name=Wemos
Configuration saved: ssid=____;password=____;server=192.168.0.3;port=5679;name=Wemos
setConfigToMap(), Key: ssid, Value: ____ set to ConfigMap
setConfigToMap(), Key: password, Value: ____ set to ConfigMap
setConfigToMap(), Key: server, Value: 192.168.0.3 set to ConfigMap
setConfigToMap(), Key: port, Value: 5679 set to ConfigMap
setConfigToMap(), Key: name, Value: Wemos set to ConfigMap
Config saved!
Client connected, IP: 192.168.4.2
````


### Library

````c
class NodeConfigClass
{
private:

protected:


public:
	/*
		Beim Start der App werden die Konfigurationsdaten aus dem Configfile
		in eine Map eingelesen.
	*/
	bool init();

	/*
		Registrierung der Callbackmethode beim CommandInterpreter.
	*/
	void registerNodeConfigCommandCallback();

	/*
		Es wird versucht, den Node mit den Zugangsdaten aus der Config am WLAN
		anzumelden. Gelingt dies nicht, wird vom Node ein eigener Accesspoint
		angelegt. Über diesen kann dann die Konfiguration erfolgen.
	*/
	void setupWifi();


	/*
		Der Konfigurationsstring wird per Http-Get empfangen
		oder aus der Konfigurationsdatei ausgelesen und in
		die Map der ConfigItems übertragen.
	*/
	bool setConfigByConfigString(String configString);

	/*
		Liest das Konfigurationselement aus der Map aus.
		Key ist nicht case-sensitiv.
	*/
	String getConfigItem(String key);

	/*
		Anzahl der Konfigurationselemente in der Map
	*/
	int getSize();
};

extern NodeConfigClass NodeConfig;
````


