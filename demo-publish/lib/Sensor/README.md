# Library Sensor

## Ziele

Abstrakte Basisklasse zur Verwaltung der allgemein gültigen Daten und Verhaltens von Sensoren.
Jeder Sensor ist innerhalb des Nodes über ID eindeutig identifizierbar. Er liefert seinen Messwert automatisch per Log an den Server, wenn er sich um eine per Parameter festgelegte Schwelle geändert hat. Er meldet seinen Messwert aber zumindest alle 15 Minuten.

## Software

### Abhängigkeiten

- Logger


### Testanwendung am ESP8266

Die Klasse Sensor wird über das Testprogramm der Library Node mitgetestet.


#### Library


#### Headerdatei Sensor.h

````c
/*
	Ein Sensor wird im System über den Nodenamen und die Sensor-ID identifiziert.
	Der Sensor verwaltet auch die Einheit seines Messwertes und die Änderungsschwelle,
	die erreicht werden muss, um eine neue Meldung des Messwertes auszulösen.

	Sensor ist eine abstrakte Klasse. Die konkreten Sensorklassen müssen nur mehr die
	Methode measure() überschreiben, mit deren Hilfe der konkrete Code zum Einlesen 
	der Messwerte implementiert wird.
*/
class Sensor
{
public:
	/*
		Sensor wird mit seinem Namen, einer innerhalb des Nodes eindeutigen id,
		der Einheit des Messwertes und der Schwelle, ab der eine Änderung des 
		Messwertes gemeldet wird.
	*/
	Sensor(String name, int id, String unit, float threashold);

	/*
		Sensor liefert den Text, mit dem er beim Server registriert wird.
	*/
	String getRegistrationString();

	/*
		Liefert den letzten gemessenen Messwert. Dieser muss nicht mit dem 
		aktuellen Messwert übereinstimmen, da sich der aktuelle Wert innerhalb
		der Schwelle geändert haben kann und daher noch nicht übermittelt wurde.
	*/
	float getLastMeasurement();

	/*
		Liefert die Id des Sensors
	*/
	int getId();

	/*
		Liefert den Namen des Sensors
	*/
	String getName();


	//	virtual void measure() = 0;  // Funktioniert in LinkedList nicht.
	/*
		(Abstrakte) Messmethode muss vom konkreten Sensor überschrieben (implementiert)
		werden.
	*/
	virtual void measure();  // abstrakte Sensoren können nicht in Liste verwaltet werden

protected:
	/*
		Aktueller Messwert wird eingetragen. Löst bei ausreichender Änderung
		Notification aus.
	*/
	void setMeasurement(float value);

private:
	String _name;
	int _id;
	String _unit;
	float _threashold;
	float _measurement;		 // letzter übertragener Wert
	float _lastMeasurement;  // letzter gemessener Wert
	long _time;
};

````

### Beispiel für einen konkreten Temperatursensor

Am Beispiel des Temperatursensors DHT22 wird die Verwendung der Klasse Sensor demonstriert.

Der Temperatursensor leitet von der Klasse Sensor ab und überschreibt die Methode zum Ermitteln des Messwertes.

````c
#include <DHT.h>
#include <Sensor.h>

class Dht22_Temperature : public Sensor
{
public:
	Dht22_Temperature(String name, int id, String unit, float threashold, uint8_t pinNumber);
	virtual void measure();
};

````

Die Implementierung benötigt ebenfalls nur einige Zeilen Code

````c
DHT dht;

Dht22_Temperature::Dht22_Temperature(String name, int id, String unit, float threashold, uint8_t pinNumber) :Sensor(name, id, unit, threashold)
{
	Serial.print("Sensor initialized: ");
	Serial.println(name);
	dht.setup(pinNumber);
}

void Dht22_Temperature::measure()
{
	float temperature = dht.getTemperature();
#ifdef Dht22_Temperature_DEBUG
	Serial.print("Temperature: ");
	Serial.println(temperature);
#endif
	setMeasurement(temperature);

}

````

Im Hauptprogramm wird der Sensor instanziert und im Node eingetragen.

````c
void setup() {
	...
	if (WifiStation.isConnected())
	{
		...
		Sensor* sensorPtr = new Dht22_Temperature("Temperature", 11, "Grad", 0.2, D1);
		Node.addSensor(sensorPtr);
	}

}

````

