// WifiHttpServer.h
#pragma once

#include <Arduino.h>
#include <ESP8266WebServer.h>

static char PHSEM_AP_NAME[] = "GyroMachine";

class HttpServerClass : public ESP8266WebServer
{
public:
	HttpServerClass(int port = 80);
	void init();

private:
};

extern HttpServerClass HttpServer;
