/*******************************************************************************
MAX31855 Thermistor Digitizer Driver
Leeman Geophysical LLC

Based on drivers written by SparkFun and Adafruit without the branding in the
name and without different things about each we felt could be changed in terms
of the API.
*******************************************************************************/

#ifndef MAX31855_H
#define MAX31855_H

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class MAX31855
{
  public:
    // Constructors
    MAX31855(const uint8_t _cs, const uint8_t _sclk, const uint8_t _miso);
    MAX31855(const uint8_t _cs);
    // Methods
    void start(void);
    float readCelcius(void);
    float readFarenheit(void);
    float readKelvin(void);
    float readColdJunction(void);
    uint8_t faultCheck(void);
    void stop(void);
  private:
    bool initialized;
    uint8_t sclk, miso, cs;
    // Error codes
    const uint8_t ERROR_OC_FAULT = 1;
    const uint8_t ERROR_SCG_FAULT = 2;
    const uint8_t ERROR_SCV_FAULT = 3;
    const uint8_t ERROR_UNKNOWN_FAULT = 4;

  protected:
    union { // Union makes conversion from 4 bytes to an unsigned 32-bit int easy
      uint8_t read_bytes[4];
      uint32_t uint32;
    } data;
    void readFourBytesSPI(void);
    uint8_t readByteSPI(void);

};

#endif
