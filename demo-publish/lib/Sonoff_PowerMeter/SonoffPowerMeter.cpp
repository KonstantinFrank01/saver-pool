#include <SonoffPowerMeter.h>
#include <Logger.h>

#define SEL_PIN 5
#define CF1_PIN 13
#define CF_PIN 14
HLW8012 hlw8012;

// Set SEL_PIN to HIGH to sample current
// This is the case for Itead's Sonoff POW, where a
// the SEL_PIN drives a transistor that pulls down
// the SEL pin in the HLW8012 when closed
#define CURRENT_MODE HIGH

// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR 0.001
#define VOLTAGE_RESISTOR_UPSTREAM (5 * 470000) // Real: 2280k
#define VOLTAGE_RESISTOR_DOWNSTREAM (1000)	 // Real 1.009k

SonoffPowerMeter::SonoffPowerMeter(const char *nodeName,
								   const char *name, const char *unit, float threashold)
	: Sensor(nodeName, name, unit, threashold)
{
	Serial.print(F("*PM: Sensor initialized: "));
	Serial.println(name);
	// Initialize HLW8012
	// void begin(unsigned char cf_pin, unsigned char cf1_pin, unsigned char sel_pin, unsigned char currentWhen = HIGH, bool use_interrupts = false, unsigned long pulse_timeout = PULSE_TIMEOUT);
	// * cf_pin, cf1_pin and sel_pin are GPIOs to the HLW8012 IC
	// * currentWhen is the value in sel_pin to select current sampling
	// * set use_interrupts to true to use interrupts to monitor pulse widths
	// * leave pulse_timeout to the default value, recommended when using interrupts
	hlw8012.begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, true);

	// These values are used to calculate current, voltage and power factors as per datasheet formula
	// These are the nominal values for the Sonoff POW resistors:
	// * The CURRENT_RESISTOR is the 1milliOhm copper-manganese resistor in series with the main line
	// * The VOLTAGE_RESISTOR_UPSTREAM are the 5 470kOhm resistors in the voltage divider that feeds the V2P pin in the HLW8012
	// * The VOLTAGE_RESISTOR_DOWNSTREAM is the 1kOhm resistor in the voltage divider that feeds the V2P pin in the HLW8012
	hlw8012.setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);
	setInterrupts();
	_actIndex = 0;
	_lastMeasurementMilliSecond = millis();
	for (int i = 0; i < POWER_MEASUREMENTS; i++)
	{
		_powerMeasurements[i] = 0;
	}
}

/**
 * Liefert alle 3 Sekunden einen neuen Messwert. Kürzere
 * Intervalle (< 2 Sekunden) führen zu Fehlmessungen. Es werden 
 * jeweils 5 Messwerte genommen, der höchste und der niedrigste 
 * Wert gestrichen. Von den 3 verbleibenden Werten wird der
 * Mittelwert verwendet.
 */
void SonoffPowerMeter::measure()
{
	if (millis() - _lastMeasurementMilliSecond < 3000)
	{
		return;
	}
	_lastMeasurementMilliSecond = millis();
	unsigned int powerMeasured = hlw8012.getActivePower();
	if (_actIndex == POWER_MEASUREMENTS)
	{
		_actIndex = 0;
	}
	_powerMeasurements[_actIndex] = powerMeasured;
	_actIndex++;
	int min = 100000;
	int max = -1;
	int sum = 0;
	for (int i = 0; i < POWER_MEASUREMENTS; i++)
	{
		sum += _powerMeasurements[i];
		if (_powerMeasurements[i] > max)
		{
			max = _powerMeasurements[i];
		}
		if (_powerMeasurements[i] < min)
		{
			min = _powerMeasurements[i];
		}
	}
	unsigned int power = (sum - min - max) / (POWER_MEASUREMENTS - 2);
	// char logText[40];
	// sprintf(logText, "%d %d %d %d", power, sum, min, max);
	// Logger.log("PowerMeter", LOG_TYPE_DEBUG, logText);
	setMeasurement(power);
}

// When using interrupts we have to call the library entry point
// whenever an interrupt is triggered
void ICACHE_RAM_ATTR hlw8012_cf1_interrupt()
{
	hlw8012.cf1_interrupt();
}
void ICACHE_RAM_ATTR hlw8012_cf_interrupt()
{
	hlw8012.cf_interrupt();
}

// Library expects an interrupt on both edges
void SonoffPowerMeter::setInterrupts()
{
	attachInterrupt(CF1_PIN, hlw8012_cf1_interrupt, CHANGE);
	attachInterrupt(CF_PIN, hlw8012_cf_interrupt, CHANGE);
}

void SonoffPowerMeter::calibrate()
{
	// Let some time to register values
	unsigned long timeout = millis();
	while ((millis() - timeout) < 10000)
	{
		delay(1);
	}
	// Calibrate using a 60W bulb (pure resistive) on a 230V line
	hlw8012.expectedActivePower(60.0);
	hlw8012.expectedVoltage(230.0);
	hlw8012.expectedCurrent(60.0 / 230.0);

	// Show corrected factors
	Serial.print("[HLW] New current multiplier : ");
	Serial.println(hlw8012.getCurrentMultiplier());
	Serial.print("[HLW] New voltage multiplier : ");
	Serial.println(hlw8012.getVoltageMultiplier());
	Serial.print("[HLW] New power multiplier   : ");
	Serial.println(hlw8012.getPowerMultiplier());
}
