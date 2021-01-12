#include <CanSatKit.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme; // I2C BME280 sensor declaration
using namespace CanSatKit;

long counter = 1;
bool led_state = false;
const int led_pin = 13;
unsigned long currentMillis = 0, previousMillis = 0, previousMillisTransmit = 0; 
float ambientPressure, ambientTemp, ambientHumidity;
const long measurementInterval = 500;           // interval at which to read sensor (milliseconds)
const long transmitInterval = 1000;           // interval at which to transmit data (milliseconds)

const float Psi2Pascal = 6894.75729F;

Radio radio(Pins::Radio::ChipSelect,
            Pins::Radio::DIO0,
            433.0,
            Bandwidth_125000_Hz,
            SpreadingFactor_9,
            CodingRate_4_8);

// create (empty) radio frame object that can store data
// to be sent via radio
Frame frameCsvValuePrintin, frameFineValuePrinting;

void setup() {
  SerialUSB.begin(115200);
  while(!Serial);    // time to get serial running

  unsigned status;
  status = bme.begin(0x76);
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280,\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
      }
  
  pinMode(led_pin, OUTPUT);

  radio.begin();
}

void loop() {
  currentMillis = millis();
  
  if (currentMillis - previousMillis >= measurementInterval) {
    previousMillis = currentMillis;

    ambientTemp = bme.readTemperature();
    ambientPressure = bme.readPressure();
    ambientHumidity = bme.readHumidity();
    

  }
  if (currentMillis - previousMillisTransmit >= transmitInterval) {
    previousMillisTransmit = currentMillis;
    digitalWrite(led_pin, led_state);
    led_state = !led_state;
  
    // store ("print") value of counter variable into radio frame
    /*currentTime.print(counter);
    
    // increment counter variable
    counter++;*/
  
//     store BME280 values in radio frame in csv value 
    frameCsvValuePrintin.print(counter);
    frameCsvValuePrintin.print(";");    
    frameCsvValuePrintin.print(currentMillis);
    frameCsvValuePrintin.print(";");
    frameCsvValuePrintin.print(ambientPressure);
    frameCsvValuePrintin.print(";");
    frameCsvValuePrintin.print(ambientTemp);
    frameCsvValuePrintin.print(";");
    frameCsvValuePrintin.print(ambientHumidity );
//    frameCsvValuePrintin.println();
  
//     store BME280 values in radio frame in fine value
    frameCsvValuePrintin.print(counter);
    frameCsvValuePrintin.print(";");  
    frameFineValuePrinting.print( "TS [s]: " );
    frameFineValuePrinting.print(currentMillis);
    frameFineValuePrinting.print("\t AP [Pa]: ");
    frameFineValuePrinting.print(ambientPressure);
    frameFineValuePrinting.print("\t AT [*C]: ");
    frameFineValuePrinting.print(ambientTemp);
    frameFineValuePrinting.print("\t AH [%]: ");
    frameFineValuePrinting.print(ambientHumidity );
    //frameFineValuePrinting.println();
  
    counter++;
    
//     send frame via radio
    while (not radio.transmit(frameCsvValuePrintin))
    {
      radio.transmit(frameCsvValuePrintin);
    }

    /* while (not radio.transmit(frameFineValuePrinting))
    {
      radio.transmit(frameFineValuePrinting;
    }*/
  
    // clear frame to make it ready for new data
    frameCsvValuePrintin.clear();
    frameFineValuePrinting.clear();
  

  }
}
