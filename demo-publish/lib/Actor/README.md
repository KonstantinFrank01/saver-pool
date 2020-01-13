# Library Actor

## Allgemeines

Abstrakte Basisklasse zur Verwaltung der allgemein gültigen Daten und Verhaltens von Aktoren.
Jeder Aktor ist innerhalb des Nodes über ID eindeutig identifizierbar. Für die Verwaltung des Zustandes des Aktors enthält jeder Aktor einen Sensor mit der selben ID und dem selben Namen. Der gewünschte Wert des Aktors kann per Methode gesetzt werden. Dieser gesetzte Wert muss nicht immer mit dem aktuellen Wert (Sensor des Aktors) übereinstimmen. Zum Beispiel kann der gewünschte Wert eines Rollladenaktors bei 50% Behanghöhe eingestellt werden, der aktuelle Wert allerdings 0% betragen. Es dauert dann einige Sekunden, bis der Wert erreicht wird. Ähnliches gilt bei der Steuerung von Temperaturen, Füllständen, ...



## Software

### Abhängigkeiten

- Logger


### Testanwendung am ESP8266

Die Klasse Sensor wird über das Testprogramm der Library Node mitgetestet.


#### Library


#### Headerdatei Actor.h

````c
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
	 Actor(String name, int id);

	 /*
		Sensor liefert den Text, mit dem er beim Server registriert wird.
	 */
	 String getRegistrationString();

	 /*
		Actor wird auf neuen Wert gesetzt.
	 */
	 void setValue(float settedValue);

	 /*
		Spezieller Actor wird aufgefordert, den gewünschten Zielwert zu setzen,
		falls er vom aktuellen Wert abweicht.
	 */
	 virtual void syncValue();

	 /*
		Liefert den aktuellen Wert des Actors
	 */
	 float getActualValue();

	 /*
		Liefert den zuletzt gesetzten Wert des Actors.
	 */
	 float getSettedValue();

	 int getId();

	 String getName();

	 /*
		Liefert einen Zeiger auf den im Actor verwalteten Sensor
	 */
	 Sensor* getSensor();

protected:
	String _name;
	int _id;
	long _time;
	float _settedValue;
	Sensor* _sensorPtr;
};


````

### Beispiel für einen konkreten Actor PowerSwitch

Ein Powerswitch schaltet per SSR die Netzspannung ein und aus. Die Ansteuerung erfolgt über einen digitalen Ausgang mit Vorwiderstand.

PowerSwitch leitet von Actor ab und hat als Spezialität nur die Pinnummer des digitalen Ausgangs.


````c
/*
	PowerSwitch ist ein spezieller sehr einfacher Aktor als
	Ein/Ausschalter über einen digitalen Ausgang.
*/
class PowerSwitch : public Actor
{
 public:
	 PowerSwitch(String name, int id, int pinNumber);
	 virtual void syncValue();

private:
	int _pinNumber;
};


````

Die Implementierung der Methoden ist auch nicht aufregend.

````c
/*
	Der Schalter hat keine Einheit des Messwertes EIN/AUS und auch eine fixe Schwelle von 0.001,
	da sowieso nur die Werte 0 und 1 verwendet werden.
*/
PowerSwitch::PowerSwitch(String name, int id, int pinNumber) : Actor(name, id) {
	_sensorPtr = new PowerSwitchSensor(name, id, "", 0.001, pinNumber);
	_pinNumber = pinNumber;
	pinMode(_pinNumber, OUTPUT);
}



void PowerSwitch::syncValue() {
	//Serial.println("In sync powerswitch");
	if (abs(getActualValue() - _settedValue) > 0.01 ) {
		if (_settedValue > 0.001) {
			digitalWrite(_pinNumber, HIGH);
		}
		else {
			digitalWrite(_pinNumber, LOW);
		}
#ifdef PowerSwitch_DEBUG
		Serial.print("Actor geaendert, ");
		Serial.print(_name);
		Serial.print(": von ");
		Serial.print(getActualValue(), 0);
		Serial.print(" nach ");
		Serial.print(_settedValue, 0);
		Serial.print(", Time: ");
		Serial.println(_time);
#endif
		Logger.log(_id, Logger.ACTORVALUE, String(_settedValue) + "/" + String(getActualValue()));
	}
}

````

Zur Verwaltung des aktuellen Zustands des Powerswitch wird ein eigener Sensor (PowerSwitchSensor) angelegt, der wider sehr einfach ist:

````c
/*
	Der PowerSwitchSensor ist der zum Actor PowerSwitch zugehörige Sensor.
	Damit wird der aktuelle Zustand des digitalen Schaltpins ausgelesen.
*/
class PowerSwitchSensor : public Sensor
{

public:
	PowerSwitchSensor(String name, int id, String unit, float threashold, uint8_t pinNumber);
	virtual void measure();

private:
	int _pinNumber;

};

````


Implementierung überschreibt die Methode Measure() und liest den aktuellen Zustand des IO-Pins aus.

````c
#include <PowerSwitchSensor.h>

PowerSwitchSensor::PowerSwitchSensor(String name, int id, String unit, float threashold, uint8_t pinNumber) 
	:Sensor(name, id, unit, threashold)
{
	Serial.print("Sensor initialized: ");
	Serial.println(name);
	_pinNumber = pinNumber;
}

void PowerSwitchSensor::measure()
{
	setMeasurement(digitalRead(_pinNumber));
}

````
