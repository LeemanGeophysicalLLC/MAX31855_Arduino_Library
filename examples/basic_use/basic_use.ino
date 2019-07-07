#include <MAX31855.h>

const uint8_t PIN_CS = 10;

// Uses hardware SPI - there is a constructor for bit-banging also
MAX31855K thermocouple(PIN_CS);

void setup()
{
  Serial.begin(9600);
  Serial.println("MAX31855 Demo Sketch");
  Serial.println("Leeman Geophysical LLC");
  delay(50);  // Delay for a few readings so we get good data
}

void loop()
{
  float cold_junction_temperature = thermocouple.readColdJunction();
  float probe_temperature = thermocouple.readCelcius();
  // Also readFarenheit and readKelvin are available
  if (!isnan(cold_junction_temperature))
  {
    Serial.print("Cold Junction: ");
    Serial.println(cold_junction_temperature)
  }

  else
  {
    Serial.println("CJT returned as NAN");
  }

  if (!isnan(probe_temperature))
  {
    Serial.print("Probe Temperature: ");
    Serial.println(probe_temperature);
  }

  else
  {
    Serial.println("Probe returned as NAN");
  }

  delay(1000);
}
