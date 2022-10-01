#include <OneWire.h>
 #include <DallasTemperature.h>

float temp = 0.0;
int oneWireBus = 12;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void setup() {
     Serial.begin(115200);
     Serial.println("Bas on Tech - 1-wire temperature sensor");
     sensors.begin();
 }

 // repeat infinitely
void loop() {
 
     sensors.requestTemperatures();
     temp = sensors.getTempCByIndex(0);
 
     Serial.print("Temperature is: ");
     Serial.println(temp);
 
     delay(500);
 
 }
