#include <FS.h> //this needs to be first, or it all crashes and burns...
#include "HttpServer.h"
#include <InternLed.h>
#include <WiFiManager.h>
//#include <GyroConfig.h>
//#include <ArduinoOTA.h>

HttpServerClass::HttpServerClass(int port) : ESP8266WebServer(port)
{
}

/*************************************** Callback-Funktionen für den Webserver ******************************/

/**
 * Verarbeitung der Übermittlung der Zugangsdaten
 * Format: http://192.168.0.12/setconfig?room=aaaa&server=www.domain.com
 */
void handleSetMqttConfigRequest()
{
	Serial.println(F("*HS: handleSetMqttConfigRequest()"));
	String server = HttpServer.arg("server");
	String room = HttpServer.arg("room");
	char serverChars[200];
	char roomChars[100];

	int changes = 0;
	if (!server.equals(""))
	{ //Parameter found
		Serial.println(F("*HS: server.toCharArray()"));
		server.toCharArray(serverChars, 200);
		//GyroConfig.setGyroServerAddress(serverChars);
		changes++;
	}
	if (!room.equals(""))
	{
		Serial.println(F("*HS: room.toCharArray()"));
		room.toCharArray(roomChars, 100);
		//GyroConfig.setGyroRoom(roomChars);
		changes++;
	}
	if (changes > 0)
	{
		//GyroConfig.saveConfig();
	}
	char response[50];
	sprintf(response, "Changed Config-Items: %d", changes);
	Serial.print("*HS: ");
	Serial.println(response);
	HttpServer.send(200, "text/html", response); //Returns the HTTP response
}

/*
	Umwandlung einer Hexziffer (0-15) in das Hexzeichen (0-F)
*/
char getHexChar(int digit)
{
	if (digit < 10)
	{
		return '0' + digit;
	}
	return 'A' + digit - 10;
}

/*
  MAC-Adresse in entsprechenden String umwandeln
  und in übergebenes char[] speichern
*/
void getMacAddress(char hexChars[])
{
	uint8_t *bssid = WiFi.BSSID();
	for (int i = 0; i < 6; i++)
	{
		hexChars[i * 3] = getHexChar(bssid[i] / 16);
		hexChars[i * 3 + 1] = getHexChar(bssid[i] % 16);
		hexChars[i * 3 + 2] = ':';
	}
	hexChars[17] = 0;
}

/*
	ESP meldet Statusinformationen, wie Mac-Adresse, SSID, WLAN-Funk-Feldstärke,
	freien Heap-Speicher, den eigenen Servernamen, Room namen
		http://192.168.0.100/state
*/
void handleStateRequest()
{
	char hexChars[18];
	char response[400];
	getMacAddress(hexChars);

	sprintf(response, "espMacAddress: %s<br/>\nwifiName: %s<br/>\nwifiStrength: %d<br/>\nfreeHeap: %d<br/>\nGyro-Server: %s<br/>\nGyro-Room: %s<br/>\n",
			hexChars, WiFi.SSID().c_str(), WiFi.RSSI(), ESP.getFreeHeap(), GyroConfig.getGyroServerAddress(), GyroConfig.getGyroRoom());

	Serial.printf("*HS response-length: %d", strlen(response));
	Serial.println("");
	Serial.print(F("*HS: State: "));
	Serial.println(response);
	//Returns the HTTP response
	HttpServer.send(200, "text/html", response);
}

/**
 * Get the current configuration
 * */
void handleGetConfigRequest()
{
	char response[100];
	//NodeConfig.getConfigText(response);
	//GyroConfig.getConfigText(response);
	Serial.print(F("*HS: State: "));
	Serial.print(response);
	//Returns the HTTP response
	HttpServer.send(200, "text/html", response);
}

/**
 * Die interne Led kann per Http gesteuert werden
 * Mit dem Parameter fast=1 blinkt die LED schnell,
 * mit slow=1 blinkt sie langsam und mit off=1 wird
 * sie abgeschaltet
 *		http://192.168.0.100/led/fast
 *		http://192.168.0.100/led/slow
 *		http://192.168.0.100/led/off
*/
void handleInternLedFastRequest()
{
	InternLed.blinkFast();
	Serial.println(F("LED blinks fast"));
	HttpServer.send(200, "text/html", "LED blinks fast");
}

void handleInternLedSlowRequest()
{
	InternLed.blinkSlow();
	Serial.println(F("LED blinks slow"));
	HttpServer.send(200, "text/html", "LED blinks slow");
}

void handleInternLedOffRequest()
{
	InternLed.blinkOff();
	Serial.println(F("LED off"));
	HttpServer.send(200, "text/html", "LED off");
}

/*
	Credentials für das WLAN werden zurückgesetzt
	SPIFFS wird formatiert
*/
void handleResetEspRequest()
{
	InternLed.blinkSlow();
	const char *response = "Formating the FlashFileSystem<br>Wifi-Connection reset settings, reboot ESP";
	HttpServer.send(200, "text/html", response); //Returns the HTTP response
	delay(1000);
	Serial.println(F("*HS Format flash memory!"));
	SPIFFS.format();
	Serial.println(F("*HS Done!"));
	WiFi.disconnect(true);
	WiFiManager wifiManager;
	Serial.println(F("*HS WifiManager resetSettings()"));
	wifiManager.resetSettings();
	Serial.println(F("*HS Done!"));
	InternLed.blinkOff();
}

/*************************************** Allgemeine HttpServer-Methoden ******************************/

//flag for saving data
bool saveConfig = false;

//callback notifying us of the need to save config
void saveConfigToFileCallback()
{
	Serial.println("*HS Should save config");
	saveConfig = true;
}

/*
	Accesspoint definieren und öffnen. Webserver starten und auf Get-Request warten
*/
void HttpServerClass::init()
{
	// Konfiguration aus Flashspeicher einlesen
	//GyroConfig.saveConfig(); // nur falsch vorher falsches config-File existiert.
	GyroConfig.readConfig();
	// damit Teil per Fing auch gefunden wird
	wifi_station_set_hostname("GyroHostName");

	// WiFiManger vorbereiten für Konfiguration per Webseite
	WiFiManagerParameter custom_server("server", "server", GyroConfig.getGyroServerAddress(), 200);
	WiFiManagerParameter custom_room("room", "room name", GyroConfig.getGyroRoom(), 100);
	WiFiManager wifiManager; // WiFiManager anlegen
	//wifiManager.autoConnect(PHSEM_AP_NAME);

	wifiManager.setSaveConfigCallback(saveConfigToFileCallback);

	wifiManager.addParameter(&custom_server);
	wifiManager.addParameter(&custom_room);

	// Namen des AP mit Suffix MAC-Adresse erzeugen
	char hexChars[18]; // Macadresse ermitteln
	getMacAddress(hexChars);
	char apName[25];
	sprintf(apName, "ESP_%s", hexChars); // SSID zusammenstoppeln
	InternLed.blinkFast();				 // Signal dafür, dass ESP mit eigenem AP auf 192.168.4.1 wartet

	bool isConnected = true;
	wifiManager.setConfigPortalTimeout(240);

	if (!wifiManager.autoConnect(PHSEM_AP_NAME)) // gecachte Credentials werden verwendet
	{
		// Serial.println(F("Failed to connect. Reset and try again..."));
		isConnected = false;
		// delay(3000);
		// ESP.reset();						//reset and try again
		// delay(5000);
	}
	delay(100);
	if (saveConfig)
	{
		InternLed.blinkSlow();
		// read updated parameters
		// save to configfile
		Serial.println(F("*HS: Save config-values to file"));
		GyroConfig.setGyroRoom(custom_room.getValue());
		GyroConfig.setGyroServerAddress(custom_server.getValue());

		GyroConfig.saveConfig();
		delay(2000);
		InternLed.blinkOff();
	}
	if (isConnected)
	{
		Serial.println(F("*HS: Connected to WiFi!"));
		Serial.print(F("*HS: Hostname: "));
		Serial.println(wifi_station_get_hostname());
		InternLed.blinkOff();
		//InternLed.blinkSlow();
		on("/setconfig", handleSetMqttConfigRequest); // Zugangsdaten per Web setzen
		on("/reset", handleResetEspRequest);		  // ESP resetten
		on("/state", handleStateRequest);			  // Status (Heap, Funkfeldstärke, ...) ausgeben
		on("/led/fast", handleInternLedFastRequest);
		on("/led/slow", handleInternLedSlowRequest);
		on("/led/off", handleInternLedOffRequest);
		on("/getconfig", handleGetConfigRequest);
		begin();
		// Serial.println(F("*HS: Webserver started"));
		delay(3000);
		Serial.println(F("*HS: Webserver started"));
	}
	else
	{
		Serial.println(F("!HS: not connected to WiFi!"));
		delay(3000);
		ESP.reset(); //reset and try again
	}
}

// Quasi Singletonimplementierung
HttpServerClass HttpServer;
