#SimpleBMP180 library

This is bare bone BMP280 Library
The library uses 32 bit fixed point calculations.
The sensor is set at the ultra low power setting and a 1 second refresh rate.

##Dependency

This library uses the Wire library.

##Usage

* begin()

 Check the device and retrieves the calibration data.  Returns false if the operation fails.

* getPressure()

 Returns the pressure in Pascal.
 
* getLastTemperature()

 Retrieves the sensor temperature in centi Celsius as calculated during the pressure reading.

See the example how to use this library.

 