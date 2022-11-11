// Fill-in information from your Blynk Template here
//#define BLYNK_TEMPLATE_ID           "TMPLxxxxxx"
//#define BLYNK_DEVICE_NAME           "Device"

const char* ssid = "Choll_2.4G";      // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "0997122060";  // The password of the Wi-Fi network

#define BLYNK_TEMPLATE_ID "TMPLx4B5yTBK"
#define BLYNK_DEVICE_NAME "Autxygen"


#define BLYNK_FIRMWARE_VERSION "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

#include "Max6675.h"
#include <WiFi.h>

#define ledpin D5

// Uncomment your board, or configure a custom board in Settings.h
//#define USE_SPARKFUN_BLYNK_BOARD
//#define USE_NODE_MCU_BOARD
//#define USE_WITTY_CLOUD_BOARD
//#define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"

Max6675 ts(21, 22, 23);

BlynkTimer timer;

// void blynk_sender_timer();


double get_temperature;

void setup() {
  Serial.begin(115200);  // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  WiFi.begin(ssid, password);  // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());  // Send the IP address of the ESP8266 to the computer


  ts.setOffset(0);
  // set offset for temperature measurement.
  // 1 stannds for 0.25 Celsius

  pinMode(ledpin, OUTPUT);

  BlynkEdgent.begin();
  timer.setInterval(1000L, blynk_sender_timer);
}

// unsigned long dht_sender_timer_premillis = 0;
// int dht_sender_timer_interval = 500;
// void dht_initial(){
//   if(millis() - dht_sender_timer_premillis >= dht_sender_timer_interval)
//     dht_sender_timer_premillis = millis();

//     get_temperature = ts.getCelsius();

// }

// unsigned long blynk_sender_timer_premillis = 0;
// int blynk_sender_timer_interval = 500;
void blynk_sender_timer() {

  // if(millis() - blynk_sender_timer_premillis >= blynk_sender_timer_interval)
  // blynk_sender_timer_premillis = millis();
  float a = ts.getCelsius();
              Blynk.virtualWrite(V0, a);
    Serial.println(a);
}

void loop() {

  // dht_initial();

  // blynk_sender_timer();

  //Serial.println(ts.getCelsius());

  if ((ts.getCelsius()) > 35) {
    digitalWrite(ledpin, HIGH);
  } else {
    digitalWrite(ledpin, LOW);
  }

  BlynkEdgent.run();
  timer.run();
}