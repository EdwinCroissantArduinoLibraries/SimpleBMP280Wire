#include <SimpleBMP280Wire.h>
#include "Arduino.h"
#include <Wire.h>

/* This is an example for the BMP280 barometric pressure sensor
 * The operation mode mode is set to to:
 * t_sb = 101 = 1000 ms
 * filter = 000 = off
 * osrs_t = 001 = 1
 * osrs_p = 001 = 1
 * mode = 11 = normal mode
 */

SimpleBMP280Wire baro; // for I2C address 0x76

// SimpleBMP280Wire baro(0x77); // for I2C address 0x77


void setup() {
	Serial.begin(9600);
	Wire.begin();
	Wire.setClock(400000L);

	if (!baro.begin()) {
		Serial.println(F("No BMP280 sensor found!"));
		while (1) {
		}
	}
}

void loop() {

	Serial.print(F("Pressure = "));
	Serial.print(baro.getPressure(), DEC);
	Serial.println(F(" Pa"));

	Serial.print(F("Temperature = "));
	Serial.print(baro.getLastTemperature(), DEC);
	Serial.println(F(" cC"));

	Serial.println();
	delay(1000);
}

