/*******************************************************************************
MAX31855 Thermistor Digitizer Driver
Leeman Geophysical LLC

Based on drivers written by SparkFun and Adafruit without the branding in the
name and without different things about each we felt could be changed in terms
of the API.
*******************************************************************************/

#include "MAX31855.h"
#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

#include <stdlib.h>
#include <SPI.h>

MAX31855::MAX31855(const uint8_t _cs, const uint8_t _sclk, const uint8_t _miso)
{
  sclk = _sclk;
  cs = _cs;
  miso = _miso;

  initialized = false;
}

MAX31855::MAX31855(const uint8_t _cs)
{
  cs = _cs;
  miso = sclk = -1;

  initialized = false;
}

void MAX31855::start(void)
{
  pinMode(cs, OUTPUT);
  digitalWrite(cs, OUTPUT);

  // For hardware SPI configure and start
  if (-1 == sclk)
  {
    SPI.begin();
  }
  // Otherwise setup the pins and bit-bang
  else
  {
    pinMode(sclk, OUTPUT);
    pinMode(miso, INPUT);
  }
  initialized = true;
}

float MAX31855::readCelcius(void)
{
  readFourBytesSPI();

  if (faultCheck())
  {
    return NAN;
  }

  int16_t temperature;

  // Bits 31-18 are thermocouple data (14-bits)
  if (data.uint32 & ((uint32_t)1 << 31))  // Sign extend negative numbers
  {
    temperature = 0xC000 | ((data.uint32 >> 18) & 0x3FFF);
  }
  else
  {
    temperature = data.uint32 >> 18; // Shift off all but the temperature data
  }

  // LSB is 0.25 degC
  return temperature * 0.25;
}

float MAX31855::readFarenheit(void)
{
  return readCelcius() * 9.0 / 5.0 + 32.0;
}

float MAX31855::readKelvin(void)
{
  return readCelcius() + 273.15
}

float MAX31855::readColdJunction(void)
{
  float temperature;

  readFourBytesSPI();

  if (faultCheck())
  {
    return NAN;
  }

  // Bits 15-4 are cold junction reference data (12-bits)
  if (data.uint32 & ((int32_t)1 << 15))  // Sign extend negative numbers
  {
    temperature = 0xF000 | ((data.uint32 >> 4) & 0xFFF);
  }
  else
  {
    temperature = (data.uint32 >> 4) & 0xFFF;
  }

  // LSB is 0.0625 degC
  return temperature * 0.0625;
}

uint8_t MAX31855::faultCheck(void)
{
  // We cannot tell if we get all zeros or all zero temperatures - unlikely
  // but possible, so we can't really check that condition.

  // D16 High - there is a fault condition present
  if (data.uint32 & (1 << 16))
  {
    //OC Fault - No Probe is attached
    if (data.uint32 & 1)
    {
      return ERROR_OC_FAULT;
    }

    // SCG Fault - Probe shorted to ground
    if (data.uint32 & (1 << 1))
    {
      return ERROR_SCG_FAULT;
    }

    // SCV Fault - Probe shorted to power
    if (data.uint32 & (1 << 2)
    {
      return ERROR_SCV_FAULT;
    }

    // Unknown Fault
    return ERROR_UNKNOWN_FAULT;
  }

  // No fault - all is well
  return 0;
}

void MAX31855::stop(void)
{
  // Stops the SPI system if using hardware or sets eveything to high-Z
  // if you're using SPI for other things, don't call this!
  if (-1 == sclk)
  {
    SPI.end();
  }

  else
  {
    pinMode(miso, OUTPUT);
  }
  initialized = false;
  digitalWrite(cs, HIGH);  // Make sure we leave device unselected
}

void MAX31855::readFourBytesSPI(void)
{
  digitalWrite(cs, LOW);

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));

  for (int i=4; i > -1; i--)
  {
    read_data[i] = readByteSPI();
  }

  SPI.endTransaction();

  digitalWrite(cs, HIGH);
}

uint8_t MAX31855::readByteSPI(void)
{
  // Hardware SPI
  if (-1 == SCLK)
  {
    return SPI.transfer(0);
  }

  // Software bit-bang SPI
  else
  {
    uint8_t byte_read = 0;
    digitalWrite(sclk, LOW);
    delay(1);

    for (i=31; i>=0; i--) {
      digitalWrite(sclk, LOW);
      delay(1);
      byte_read <<= 1;
      if (digitalRead(miso)) {
        byte_read |= 1;
      }

      digitalWrite(sclk, HIGH);
      delay(1);
    }
  return byte_read;
  }
}
