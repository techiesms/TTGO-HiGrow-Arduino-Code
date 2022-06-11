/*
 * 
 * This is the Demo code for TTGO HiGrow Sensor 
 * 
 * Code was written by Sachin Soni for techiesms YouTube channel
 * visit our channel to know more about this sensor and other 
 * electronic projects - https://www.YouTube.com/techiesms
 * 
 * 
 * This code was test with 
 * Arduino IDE version - 1.8.19
 * ESP32 Boards packages version - 1.0.6
 * DHT Sensor library version - 1.4.3
 * BH1750 Sensor library version - 1.3.0
 * 
 */




#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>
#include <SD.h>
#include <SPI.h>
#include <Esp.h>

#define I2C_SDA 25
#define I2C_SCL 26
#define DHT_PIN 16
#define BAT_ADC 33
#define SALT_PIN 34
#define SOIL_PIN 32
#define BOOT_PIN 0
#define POWER_CTRL 4
#define USER_BUTTON 35

#define soil_max 1638
#define soil_min 3285

#define DHT_TYPE DHT11


float luxRead;
float advice;
float soil;

BH1750 lightMeter(0x23); //0x23
DHT dht(DHT_PIN, DHT_TYPE);



void setup() {
  // Debug console
  Serial.begin(115200);

  dht.begin();

  pinMode(POWER_CTRL, OUTPUT);
  digitalWrite(POWER_CTRL, 1);
  delay(1000);

  bool wireOk = Wire.begin(I2C_SDA, I2C_SCL); // wire can not be initialized at beginng, the bus is busy
  if (wireOk)
  {
    Serial.println(F("Wire ok"));
  }
  else
  {
    Serial.println(F("Wire NOK"));
  }
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
  {
    Serial.println(F("BH1750 Advanced begin"));
  }
  else
  {
    Serial.println(F("Error initialising BH1750"));
  }

  // Battery status, and charging status and days.
  float bat = readBattery();
  Serial.println("Battery level");
  Serial.println(bat);

}

void loop()
{
  Serial.print("**********************************");
  Serial.print("Printing Data of ALl the Sensors");
  Serial.println("**********************************");
  Serial.println("");

  luxRead = lightMeter.readLightLevel();
  Serial.print("Lux - "); Serial.println(luxRead);

  uint16_t soil = readSoil();
  Serial.print("Soil Moisture - "); Serial.println(soil);

  uint32_t salt = readSalt();
  String advice;
  if (salt < 201)
  {
    advice = "needed";
  }
  else if (salt < 251)
  {
    advice = "low";
  }
  else if (salt < 351)
  {
    advice = "optimal";
  }
  else if (salt > 350)
  {
    advice = "too high";
  }
  Serial.print("Salt Advice - "); Serial.println(advice);

  float t = dht.readTemperature(); // Read temperature as Fahrenheit then dht.readTemperature(true)
  Serial.print("Temperature - "); Serial.println(t);

  float h = dht.readHumidity();
  Serial.print("Humidity - "); Serial.println(h);

  float batt = readBattery();
  Serial.print("Battery - "); Serial.println(batt);


  delay(2000);

}


// READ Soil
uint16_t readSoil()
{
  uint16_t soil = analogRead(SOIL_PIN);
  return map(soil, soil_min, soil_max, 0, 100);
}

// READ Battery
float readBattery()
{
  int vref = 1100;
  uint16_t volt = analogRead(BAT_ADC);
  // Serial.print("Volt direct ");
  // Serial.println(volt);
  float battery_voltage = ((float)volt / 4095.0) * 2.0 * 3.3 * (vref) / 1000;
  Serial.print("Battery Voltage: ");
  Serial.println(battery_voltage);
  battery_voltage = battery_voltage * 100;
  return map(battery_voltage, 416, 290, 100, 0);
}

// READ Salt
uint32_t readSalt()
{
  uint8_t samples = 120;
  uint32_t humi = 0;
  uint16_t array[120];

  for (int i = 0; i < samples; i++)
  {
    array[i] = analogRead(SALT_PIN);
    delay(2);
  }
  std::sort(array, array + samples);
  for (int i = 0; i < samples; i++)
  {
    if (i == 0 || i == samples - 1)
      continue;
    humi += array[i];
  }
  humi /= samples - 2;
  return humi;
}
