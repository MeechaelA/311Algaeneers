/*
MAE 311 ALGAENEERS (Team 7) ARDUINO DATA COLLECTION / SYSTEM MONITORING SCRIPT
Brady Burnsides
March 2019

Used in conjunction with Python Serial Monitoring library. Logs data to CSV (Python 2)
Dependencies: OneWire and DallasTemperature libraries for thermocouple
*/

// include libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS); // MIGHT NOT NEED THIS
DallasTemperature sensors(&oneWire);

// Store readings
float tempC = 0; // thermocouple temperature reading
int CO2mv = 0; // raw CO2 sensor reading
int CO2mvScaled = 0; // interim calculation of [ppm] calculation
float CO2concen = 0; // final converted CO2 reading, in [ppm]

// Store values to print
timeString = String(0);
CO2String = String(0);
tempString = String(0);
relayStatusString = String(0);
finalString = timeString + CO2String + tempString + relayStatusString;

// Pins
int tempPin = 3; // Digital pin thermocouple is controlled via (this is not actually used and is handled by the OneWire library)
int CO2pin = A0; // Analog pin CO2 outputs to
int relayPin = 2; // Digital pin relay is controlled via

// Initialize timers
unsigned long prevTemp = 0; // Stores last time that the temp was checked for heat pad updating
unsigned long prevLog = 0; // Stores last time the data was logged
unsigned long overrideTime = 0; // Stores time the relay override was begun (allow things to cool down if heatpad is on too long)

// House keepings
int relayStatus = 0; // 0 = heat pad on, 1 = heat pad off
int logNOW = 0;
int relayOVERRIDE = 0;

// Control logging frequencies
long tempInterval = 60000; // Check temperature every 1 [min]
long logInterval = 180000; // Log data every 3 [min] 
long heatPadTimeout = 7200000; // Controls the timeout of the heating pad. 7.2M [ms] = 2 [hr]
long heatPadCooldown = 900000; // cooldown time of heatpad if things are on too long (15 [min])

// Control temperature range
int heatRangeLow = 20; // Lower bound of temperature range we want to keep water at
int heatRangeHigh = 25; // Upper bound of temperature range we want to keep water at

void setup() {
	pinMode(relayPin, HIGH); // Setup relay control pin as output
	sensors.begin(); // Start up temp sensor library
	analogReference(INTERNAL); // 1.1V reference for CO2 sensor
	Serial.begin(9600); // initialize serial logging
	tempC = sensors.requestTemperatures(); // get initial temperature reading	
}

void loop() {

	// Check temperature so that the heatpad can be turned on/off in order to maintain 5C range in water
	if ((millis() - prevTemp) >= tempInterval && relayOVERRIDE = 0) {
		tempC = sensors.getTempCByIndex(0); // get current temperature [degC]

		if (tempC >= heatRangeHigh) { // If water temperature is too high
			digitalWrite(relayPin, LOW); // Turn relay (heat pad) off
			relayStatus = 0; // Update current status of relay for data logging purposes
			logNOW = 1; // Force a data log
		}
		if (tempC <= heatRangeLOW) { // If water temperature is too low
			digitalWrite(relayPin, HIGH); // Turn relay (heat pad) on
			relayState = 1; // Update current status of relay for data logging purposes
			logNOW = 1; // Force a data log
		}
		prevTemp = millis(); // Update the previous time the temperature was checked
	}

	// Check temperature and CO2 levels for data logging purposes
	if ((millis() - prevLog) >= logInterval || logNOW == 1) {
		tempC = sensors.getTempCByIndex(0); // get current temperature [degC]
		CO2mv = analogRead(CO2pin); // get current CO2 reading [mv]
		CO2mvScaled = CO2mv*(1100/1024.0); // Scale input by reference level and Arduino ADC bits

		// Check if CO2 reading was successful
		if(CO2mvScaled == 0) {
			CO2String = String("ERR"); // Convert to a string for data logging
		}
		else if(CO2mvScaled < 400) {
			CO2String = String("PH"); // PH -> Preheating (These values usually only occur when the sensor hasn't been running for long)
		}
		else {
			CO2concen = ((CO2mvScaled - 400))*(50.0/16.0); // voltageLevel - 400 [mV] offset then a final calibration scaling for [mV] -> [ppm] 
			CO2String = String(CO2concen); // Convert [ppm] value to a string for data logging
		}

		// Check if temperature reading was successful
		if(tempC != DEVICE_DISCONNECTED_C) {
			tempString = String(tempC); // convert [degC] value to a string for data logging
		}
		else {
			tempString = String("ERR"); // Thermocouple was not properly connected so reading wasn't obtained
		}

		// Convert 0 and 1 to ON/OFF for heat pad
		if (relayStatus = 1) {
			relayStatusString = String("ON");
		}
		else {
			relayStatusString = String("OFF");		
		}

		// Compile data into CSV format for data logging
		finalString = timeString + "," + CO2String + "," + tempString + "," + relayStatusString;
		Serial.println(finalString); // Print results

		prevLog = millis();
		logNOW = 0; // Resets the value in case the logging was caused by a heat pad change. 
	}

	// Turn off heat pad if its been on too long
	if (relayStatus == 1 && (millis() - prevTemp) > heatPadTimeout) {
		digitalWrite(relayPin, LOW);
		relayStatus = 0
		relayOVERRIDE = 1;
		overrideTime = millis();
	}

	// Disable override if its been long enough
	if (relayOVERRIDE == 1 && millis() - overrideTime >= heatPadCooldown) {
		relayOVERRIDE = 0;
	}
}