#pragma once

#include <Actor.h>

/*
	SonoffSwitch ist ein spezieller sehr einfacher Aktor als
	Ein/Ausschalter über einen digitalen Ausgang.
*/
class SonoffSwitch : public Actor
{
 public:
	 SonoffSwitch(const char* nodeName, const char* name, int pinNumber);
	 virtual void changeActorTo(float value);
private:
	int _pinNumber;
};

