#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>
/////////////LED Matrix dot //////////////////////
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 5

MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
/////////////LED Matrix dot //////////////////////
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN     4 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 8// Popular NeoPixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels
int Green=230,Red=160,Blue=0;

//=================================================================================================
WiFiClient   espClient;
PubSubClient client(espClient);             //สร้างออปเจ็ค สำหรับเชื่อมต่อ mqtt
//=================================================================================================
const char* ssid = "ESL-Net";               //wifi name
const char* password = "ceeslonly";         //wifi password
//=================================================================================================
const char* mqtt_broker = "161.246.6.90";   //IP mqtt server
const char* mqtt_username = "aquarius";        //mqtt username
const char* mqtt_password = "z2honly";  //mqtt password
const int   mqtt_port = 1883;               //port mqtt server
//=================================================================================================

// Flame Sensor Module
//void isFlame_function();
//void FlameCheck();
int check=0;
#define CLK 19
#define DT 21
#define SW 34
int LED = 13; // Use the onboard Uno LED
int isFlamePin = 2;  // This is our input pin
int isFlame = HIGH;  // HIGH MEANS NO FLAME
int buzzer = 22;
//==================Set up Encoder================
int counter = 0; //// Value 
int currentStateCLK;
int lastStateCLK;
String currentDir =""; ////
unsigned long lastButtonPress = 0;
//===================================================
float temp = 0.0,roll_temp = 0.0;
char tempstr[100];
int oneWireBus = 12; // tempRead
int cooldown=0, cooldown2=0, cooldown3=0;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
void Encoder();
//////////////////////////////////////////////////////
void setup() {
     Serial.begin(115200);
     pinMode(buzzer, OUTPUT);
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'
    Display.begin();
    Display.setIntensity(0);
    Display.displayClear();
    Display.displayScroll("FLAME DETECTER", PA_RIGHT, PA_SCROLL_LEFT, 40);
  lastStateCLK = digitalRead(CLK); // Encoder  
     Serial.println("Bas on Tech - 1-wire temperature sensor");
     sensors.begin();
     pinMode(LED, OUTPUT); // put onboard LED as output
     pinMode(isFlamePin, INPUT); //flame sensor should be input as it is giving data
     setup_wifi(); //เชื่อมต่อwifi
     reconnect();  //เชื่อมต่อmqtt
     client.subscribe("hero/aquarius/sleep");  //กำหนด topic ที่จะ subscribe
     //client.publish("hero/aquarius/slum", temp); //กำหนด topic ที่จะ publish และ valu
 }   
 // repeat infinitely
 
void loop() {

//     isFlame_function();
       Encoder(); 
       if(millis()-cooldown > 40){
         if(Display.displayAnimate()){
          Display.displayReset();
        } 
        cooldown = millis();
       }
}
 ///////////////  Encoder ///////////////////////
 void Encoder(){
  currentStateCLK = digitalRead(CLK);

  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    if (digitalRead(DT) != currentStateCLK) {
      counter ++;
      roll_temp +=2;     //////// Temp_increse
      currentDir ="Right";
    } 
    else {
      if(counter>=0){
      counter --;
      roll_temp -=2;  /////// Temp_decresement
      }
      else{
       counter = 0;   
       roll_temp = 0;
       currentDir ="Left";
      }
    }
   //////////////Config Rotation /////////
    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
  }

  lastStateCLK = currentStateCLK; ///// laststate = state
  
  int btnState = digitalRead(SW);
  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
    }
    lastButtonPress = millis();
  }
  if(millis() - cooldown3 > 1){
  check++;
  if(check>5000){
        client.loop();//วนลูปรอsubscribe valu จาก mqtt
     sensors.requestTemperatures();
     char strtemp[20];  
     
     temp = sensors.getTempCByIndex(0);
     if(temp>0){
     sprintf(strtemp, "%.2f", temp+roll_temp);
     Serial.print("Temperature is: ");
     Serial.println(temp+roll_temp);
     }

     isFlame = digitalRead(isFlamePin);

     if(temp+roll_temp > 60 && isFlame == HIGH){
        Serial.println("FLAME DETECTED, PLEASE CALL FOR HELP"); //print flame
        client.publish("hero/aquarius/slumz", "FLAME DITECTED PLEASE CALL FOR HELP");
        digitalWrite(buzzer, HIGH);
        Serial.println("HIGH");
        Serial.println(digitalRead(buzzer));
        Neopixel(60,240,0,3); 
     }else if(temp+roll_temp > 60 && isFlame == LOW){
        Serial.println("FLAME DETECTED, PLEASE CALL FOR HELP"); //print flame
        client.publish("hero/aquarius/slumz", "FLAME DITECTED PLEASE CALL FOR HELP");
        digitalWrite(buzzer, HIGH);
        Serial.println("LOW");
        Serial.println(digitalRead(buzzer));
        Neopixel(90,180,0,2);    
     }else{
        Serial.println("NO FLAME DETECTED");
        client.publish("hero/aquarius/slumz", "NO FLAME DITECTED");
        digitalWrite(buzzer, LOW);
        if(isFlame == HIGH){
         Serial.println("HIGH");}
        else{
         Serial.println("LOW");}
         Serial.println(digitalRead(buzzer));
        Neopixel(185,3,15,1);            
     }

     client.publish("hero/aquarius/slum", strtemp); 
     check=0;
  }
   cooldown3 = millis();
  }
}

void setup_wifi(){   //ฟังก์ชั่นเชื่อมต่อwifi
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);              //เลือกโหมดรับ wifi
  WiFi.begin(ssid, password);       //เชื่อมต่อ wifi
  while (WiFi.status() != WL_CONNECTED)     //รอจนกว่าจะเชื่อมต่อwifiสำเร็จ
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {  //ฟังก์ชั่นsubscribe
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
    Serial.print((char) payload[i]);
  Serial.println();
  Serial.println("-----------------------");
}

void reconnect() {  //ฟังก์ชั่นเชื่อมต่อmqtt
  client.setServer(mqtt_broker, mqtt_port);   //เชื่อมต่อmqtt
  client.setCallback(callback);               //เลือกฟังก์ชั่นsubscribe
  while (!client.connected()) //รอจนกว่าจะเชื่อมต่อmqttสำเร็จ
  {
    String client_id = "esp32-New-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
      Serial.println("Public emqx mqtt broker connected");
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}
void Neopixel(int Green,int Red ,int Blue,int State){
      for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
         pixels.setPixelColor(i, pixels.Color(Green, Red, Blue));
         if(State==3){
         Green-=10;}
         else if(State==2||State==1){
         Green+=10;
         }
         pixels.show();  
     }
  if(millis()-cooldown2 > 2000){
   pixels.clear();
   cooldown2 = millis();
   }      
} /*
void Display_Matrix_Dot(int a){
 if(a==1){
    Display.displayScroll("NO FLAME DITECTED", PA_RIGHT, PA_SCROLL_LEFT, 80);  
     if (Display.displayAnimate()) {
    Display.displayReset();
    delay(1500);
  }
}
else if(a==2){
    Display.displayScroll("FLAME DITECTED, PLEASE CALL FOR HELP", PA_RIGHT, PA_SCROLL_LEFT, 60);
     if (Display.displayAnimate()) {
    Display.displayReset();
    delay(1500);
  }  
}
else if(a==3){
    Display.displayScroll("FLAME DITECTED, PLEASE CALL FOR HELP", PA_RIGHT, PA_SCROLL_LEFT, 50);
     if (Display.displayAnimate()) {
    Display.displayReset();
    delay(1500);
  }    
}
delay(4000);
    Display.displayClear();
} */
