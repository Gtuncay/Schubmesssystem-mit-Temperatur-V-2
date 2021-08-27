//Include Libraries
#include "HX711.h"
#include <SPI.h> //Datalogger
//#include <SD.h> //Daralogger
#include <Wire.h> //Dartalogger
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 3  //Sensor DS18B20 am digitalen Pin 3
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature DS18B20 (&oneWire);

//Define Reset Pin & DATA/CLK Pins of Load Cells
//#define rstPin 30 //Definition Reset Pin
#define DOUT1 5 //Data for Sensor 1 (O)
#define CLK1 2 //Clock for Sensor 1
#define DOUT2 6 //Data for Sensor 2 (U)
#define CLK2 9  //Clock for Sensor 2

//Device Initializations

HX711 scale1; //Declaration Sensor 1
HX711 scale2; //Declaration Sensor 2

unsigned long time = 0;
int sensorCount;
const int chipSelect = 53;
const int RECV_PIN = 37;
float tempc;

float calibration_factor_1 = 25000; //To measure in N Sensor1 21,5°C
float calibration_factor_2 = 25200; //To measure in N Sensor2 21,5°C
bool startprocess = false;
unsigned long synctime;

//Data for Serial Communication in Struct with byte pointer
struct datasend 
{
   unsigned long relclk;
   float temp1K;
   float temp2K;
   float force1;
   float force2;
   unsigned long raw1;
   unsigned long raw2;
};

datasend data;
//------------------------------------------------------------------------------------------Setup---------------------------------------------------------------------------------------------------------------------------------------


void setup(void) {  
 
//Beginn of serial Communication, Baud Rate is set lower Data Frecuency gets below 10Hz
	Serial.begin(500000); //500000
	DS18B20.begin(); //DS18B20 Start Communication with Temp Sensor
	sensorCount = DS18B20.getDS18Count(); //Lesen der Anzahl der angeschlossenen Temperatursensoren.
// Waiting for the starting signal of Victor
    //while (!startprocess)
    //{
		synctime = millis();
	  //	if (Serial.available())
	  //	{
	  //  		if(Serial.read()=='S')
	  //		{
	  //			Serial.print("*m");
	  //			Serial.write((byte*)&synctime, sizeof(synctime));
	  //			Serial.print("e/");
	  //			startprocess = true;
	  //		}
	  //	}
    //}

//Setup Sensor 1 -> "Oben" Force Sensor
	scale1.begin(DOUT1, CLK1);
	scale1.set_scale(calibration_factor_1); //This value is obtained by using the SparkFun_HX711_Calibration sketch
	scale1.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
 
//Setup Sensor 2 -> "Unten" Force Sensor
	scale2.begin(DOUT2, CLK2);
	scale2.set_scale(calibration_factor_2); //This value is obtained by using the SparkFun_HX711_Calibration sketch
	scale2.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

}	// END OF SETUP FUNCTION

//------------------------------------------------------------------------------------------Loop---------------------------------------------------------------------------------------------------------------------------------------

void loop(void)
{
	time = millis();
    if (Serial.available())
	{
		if(Serial.read()=='R')
		{
			startprocess = false;
			setup();
		}
	}

	//Get information Force Sensor 1
	float f1 = scale1.get_units();
	//Get information Force Sensor 2
	float f2 = scale2.get_units();
	
	DS18B20.requestTemperatures();

	//Store information in the struct
	

	data.relclk = time;  		
	data.temp1K = DS18B20.getTempCByIndex(0) + 273.15;
    data.temp2K = DS18B20.getTempCByIndex(sensorCount-1) + 273.15; //Anzahl DS18B20 - 1
	data.force1 = f1;
    data.force2 = f2;
    data.raw1 = scale1.read();
	data.raw2 = scale2.read();

	//Serial Comunication -> Struct is send 
	Serial.print("*b");
    Serial.write((byte*)&data, sizeof(data));
    Serial.print("e/");

}
