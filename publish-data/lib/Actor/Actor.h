#pragma once

#include <Sensor.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <MqttClient.h>

/*
	Die Klasse Actor dient als abstrakte Basisklasse für beliebige Typen von Aktoren.
	Jeder Actor hat einen aktuellen Zustand, der als Sensor behandelt wird.
	Weiters gibt es einen Wert, auf den der Actor gesetzt werden soll. Nach endlicher Zeit
	stimmen die beiden Werte überein, sofern der Actor korrekt funktioniert.
	Beispiel PowerSwitch:
				Der aktuelle Wert repräsentiert den Zustand des Schalters (1 für ein, 0 für aus)
				Der zu setzende Wert gibt den Zielzustand an. Wird dieser Wert per Commando umgesetzt, 
				wird innerhalb einiger Millisekunden das SSR geschaltet und auch der aktuelle Wert angepasst.
	Beispiel Rollladenschalter:
				Der aktuelle Wert zwischen 0 und 1 repräsentiert die Stellung des Rollladens (0 geschlossen, 0,5
				halb offen, 1 offen). Wird der Zielwert neu gesetzt, dauert es natürlich einige Sekunden, bis
				der aktuelle Wert den Zielwert erreicht.

	Alle speziellen Aktoren (z.B. Powerswitch) leiten von dieser Basisklasse ab initialisieren ihren 
	zugehörigen Sensor (z.B. PowerSwitchSensor) und überschreiben die Methode syncValue(), die dafür sorgt,
	dass der aktuelle Aktorwert mit dem gesetzten übereinstimmt.
*/
class Actor
{
  public:
	/*
		Der Aktor wird mit seinen Stammdaten initialisiert. Die Parameter
		unit und threashold werden an den Messwert (Sensor) des Aktors 
		weiter gereicht.
	 */
	Actor(const char *nodeName, const char *name);

	/*
		Actor wird auf neuen Wert gesetzt.
	 */
	virtual void setSettedValue(float value);

	/*
		Spezieller Actor wird aufgefordert, den gewünschten Zielwert zu setzen,
		falls er vom aktuellen Wert abweicht.
	 */
	void syncValue();

	/*
		Liefert den aktuellen Wert des Actors
	 */
	float getActualValue();

	/*
		Liefert den zuletzt gesetzten Wert des Actors.
	 */
	float getSettedValue();

	char *getName();

	/*
		Liefert einen Zeiger auf den im Actor verwalteten Sensor
	 */
	Sensor *getSensor();

	void setSettedValueAsJson(const char *payload);

  protected:
	/**
	 * Der konkrete Aktor ändert den Zustand entsprechend
	 * z.B. schaltet den Schalter ein/aus
	 */
	virtual void changeActorTo(float value)=0;

	char _nodeName[30];
	char _name[20];
	long _time;
	float _settedValue;
	Sensor *_sensorPtr;

  private:
	StaticJsonBuffer<200> _jsonBuffer;
	MqttSubscription _actorSubscription;
	char _actorTopic[50];
	bool _isStart; 
};
