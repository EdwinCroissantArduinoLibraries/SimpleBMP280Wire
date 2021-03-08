/*
 * BMP280 barometric pressure sensor library for the Arduino
 * microcontroller.
 *
 * Uses the Wire library
 *
 * Copyright (C) 2016 Edwin Croissant
 *
 *  This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See the README.md file for additional information.
 */

#include <SimpleBMP280Wire.h>

SimpleBMP280Wire::SimpleBMP280Wire(int8_t I2C_Addr) {
	_I2C_Addr = I2C_Addr;
	_lastTemp = -27315;
}

/*
 * Check the device and retrieves the calibration data
 * Set the mode to:
 * t_sb = 101 = 1000 ms
 * filter = 000 = off
 * osrs_t = 001 = 1
 * osrs_p = 001 = 1
 * mode = 11 = normal mode
 * Returns false if the operation fails.
 */
boolean SimpleBMP280Wire::begin() {
	/* check device */
	retrieveData(id, 1);
	if (Wire.read() != deviceId)
		return false;

	/* read calibration data */
	retrieveData(calData, 24);
	for (int i = 0; i < 24; ++i) {
		_calData.calArray[i] = Wire.read();
	}

// set the mode
	writeCmd(config, 0b10100000);
	writeCmd(control, 0b00100111);

	return true;
}

/*
 * Returns the pressure in Pascal.
 */
int32_t SimpleBMP280Wire::getPressure(void) {
	int32_t var1, var2, t_fine;
	uint32_t p;

	union {
		int32_t int32;
		struct {
			uint8_t LLSB, LSB, MSB, MMSB;
		};
	} UT, UP; //uncompensated temperature and uncompensated pressure

	retrieveData(data, 6);
	UP.MMSB = 0;
	UP.MSB = Wire.read();
	UP.LSB = Wire.read();
	UP.LLSB = Wire.read();
	UT.MMSB = 0;
	UT.MSB = Wire.read();
	UT.LSB = Wire.read();
	UT.LLSB = Wire.read();

	UT.int32 >>= 4;
	UP.int32 >>= 4;

	var1 = ((((UT.int32 >> 3) - ((int32_t) _calData.dig_T1 << 1)))
			* ((int32_t) _calData.dig_T2)) >> 11;

	var2 = (((((UT.int32 >> 4) - ((int32_t) _calData.dig_T1))
			* ((UT.int32 >> 4) - ((int32_t) _calData.dig_T1))) >> 12)
			* ((int32_t) _calData.dig_T3)) >> 14;

	t_fine = var1 + var2;
	_lastTemp = (t_fine * 5 + 128) >> 8;

	var1 = (t_fine >> 1) - (int32_t) 64000;

	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t) _calData.dig_P6);

	var2 = var2 + ((var1 * ((int32_t) _calData.dig_P5)) << 1);
	var2 = (var2 >> 2) + (((int32_t) _calData.dig_P4) << 16);
	var1 = (((_calData.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3)
			+ ((((int32_t) _calData.dig_P2) * var1) >> 1)) >> 18;
	var1 = ((((32768 + var1)) * ((int32_t) _calData.dig_P1)) >> 15);

	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}

	p = (((uint32_t) (((int32_t) 1048576) - UP.int32) - (var2 >> 12))) * 3125;

	if (p < 0x80000000) {
		p = (p << 1) / ((uint32_t) var1);
	} else {
		p = (p / (uint32_t) var1) * 2;
	}

	var1 = (((int32_t) _calData.dig_P9)
			* ((int32_t) (((p >> 3) * (p >> 3)) >> 13))) >> 12;
	var2 = (((int32_t) (p >> 2)) * ((int32_t) _calData.dig_P8)) >> 13;
	p = (uint32_t) ((int32_t) p + ((var1 + var2 + _calData.dig_P7) >> 4));

	return p; // in Pascal
}

/*
 * Retrieves the sensor temperature in centiCelsius as
 * measured during the pressure reading.
 */
int16_t SimpleBMP280Wire::getLastTemperature(void) {
	return _lastTemp;
}

/*********************************************************************/

void SimpleBMP280Wire::writeCmd(BMP280registers rgstr, uint8_t cmd) {
	Wire.beginTransmission(_I2C_Addr); // start the transmission
	Wire.write(rgstr); // set the register address to read from
	Wire.write(cmd);  // write the data
	Wire.endTransmission(); // end the transmission
}

uint8_t SimpleBMP280Wire::retrieveData(BMP280registers rgstr, int8_t quantity) {
	Wire.beginTransmission(_I2C_Addr); // start the transmission
	Wire.write(rgstr); // set the register address to read from
	Wire.endTransmission(); // end the transmission
	return Wire.requestFrom(_I2C_Addr, quantity); // get the data
}
