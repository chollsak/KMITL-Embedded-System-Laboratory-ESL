#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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


int LED = 13; // Use the onboard Uno LED
int isFlamePin = 2;  // This is our input pin
int isFlame = HIGH;  // HIGH MEANS NO FLAME
int buzzer = 22;


float temp = 0.0;
char tempstr[100];
int oneWireBus = 12; // tempRead

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

void setup() {
     Serial.begin(115200);
     pinMode(buzzer, OUTPUT);
     
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
     client.loop();//วนลูปรอsubscribe valu จาก mqtt
     sensors.requestTemperatures();

     char strtemp[20];
     
     
     temp = sensors.getTempCByIndex(0);
     sprintf(strtemp, "%.2f", temp);
     
     Serial.print("Temperature is: ");
     Serial.println(temp);

     isFlame = digitalRead(isFlamePin);

     if(temp > 100 && isFlame == HIGH){
        Serial.println("FLAME DITECTED, PLEASE CALL FOR HELP"); //print flame
        client.publish("hero/aquarius/slumz", "FLAME DITECTED PLEASE CALL FOR HELP");
        digitalWrite(buzzer, HIGH);
        Serial.println(digitalRead(buzzer));
        
     }else if(temp > 100 && isFlame == LOW){
        Serial.println("FLAME DITECTED, PLEASE CALL FOR HELP"); //print flame
        client.publish("hero/aquarius/slumz", "FLAME DITECTED PLEASE CALL FOR HELP");
        digitalWrite(buzzer, HIGH);
        Serial.println(digitalRead(buzzer));
        
     }else{
        Serial.println("NO FLAME DITECTED");
        client.publish("hero/aquarius/slumz", "NO FLAME DITECTED");
        digitalWrite(buzzer, LOW);
        Serial.println(digitalRead(buzzer));
        
     }

     
     client.publish("hero/aquarius/slum", strtemp);
  
     delay(500);
 
 }
  


void setup_wifi() {   //ฟังก์ชั่นเชื่อมต่อwifi
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
