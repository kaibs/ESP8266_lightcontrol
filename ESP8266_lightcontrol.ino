#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <RCSwitch.h>

#include "credentials.h"

// Connect to the WiFi
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* mqtt_server = MQTT_SERVER_IP;
const char* user= MQTT_USER;
const char* passw= MQTT_PASSWORD;

String receivedString;
//int brightnessHelper;

WiFiClient espBookshelf;
PubSubClient client(espBookshelf);

RCSwitch sender = RCSwitch();


const int led = 5;
int brightness = 122;



void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 if (strcmp(topic,"home/bedroom/bookshelf/switch")==0){
 for (int i=0;i<length;i++) {
  receivedString += (char)payload[i];
  
  
  if (receivedString == "ON"){
    Serial.println("ON");
    stripON();
  }

  if (receivedString == "OFF"){
    Serial.println("OFF");
    stripOFF();
  }
 }
  receivedString = "";
 }

 if (strcmp(topic,"home/bedroom/bookshelf/brightness")==0){
 
 for (int i=0;i<length;i++) {
  receivedString += (char)payload[i];
  
  
 }

  Serial.println("brightness: " + receivedString);
  Serial.println("Laenge: " + (String)receivedString.length());
  
  brightness = receivedString.toInt();
  Serial.println("int: " + (String)brightness);
  
  setBright();
  
  receivedString = "";
  
  }
  if (strcmp(topic,"home/bedroom/switch3")==0){
 for (int i=0;i<length;i++) {
  receivedString += (char)payload[i];
  //Serial.print(receivedString);
  if (receivedString == "ON")
  switchon_s3();
  //Serial.println("triggered");
  if (receivedString == "OFF")
  switchoff_s3();
  //Serial.println("triggered off"); 
 }
 //Serial.println();
 receivedString = "";
 }
 if (strcmp(topic,"home/bedroom/switch4")==0){
 for (int i=0;i<length;i++) {
  receivedString += (char)payload[i];
  //Serial.print(receivedString);
  if (receivedString == "ON")
  switchon_s4();
  
  if (receivedString == "OFF")
  switchoff_s4(); 
 }
 //Serial.println();
 receivedString = "";
 }
 if (strcmp(topic,"home/bedroom/lamp1")==0){
 for (int i=0;i<length;i++) {
  receivedString += (char)payload[i];
  //Serial.print(receivedString);
  if (receivedString == "ON")
  switchon_L1();
  
  if (receivedString == "OFF")
  switchoff_L1(); 
 }
 //Serial.println();
 receivedString = "";
 }
  
 }




void stripON(){

  analogWrite(led, brightness);
  String helpVal = (String)brightness;
  char feedback[10];
  helpVal.toCharArray(feedback, 10);
  client.publish("home/bedroom/bookshelf/feedback", feedback);
  
}


void stripOFF(){
  
  analogWrite(led, 0);
  char feedback[5] = "0";
  client.publish("home/bedroom/bookshelf/feedback", feedback);

}


void setBright(){

  
  analogWrite(led, brightness);
  String helpVal = (String)brightness;
  char feedback[10];
  helpVal.toCharArray(feedback, 10);
  client.publish("home/bedroom/bookshelf/feedback", feedback);
  
}

void switchon_s3(){
  sender.setProtocol(1);
  //sender.setPulseLength(425);
  sender.sendTriState("F0FF0FFFFFFF");
  //delay(5);
  Serial.println("ON pressed");
}

void switchoff_s3(){
  sender.setProtocol(1);
  //sender.setPulseLength(425);
  sender.sendTriState("F0FF0FFFFFF0");
  //delay(5);
  Serial.println("OFF pressed");
}

void switchon_s4(){
  sender.setProtocol(1);
  //sender.setPulseLength(425);
  sender.sendTriState("0FFFFFF0FFFF");
  //delay(5);
  Serial.println("ON pressed");
}

void switchoff_s4(){
  sender.setProtocol(1);
  //sender.setPulseLength(425);
  sender.sendTriState("0FFFFFF0FFF0");
  //delay(5);
  Serial.println("OFF pressed");
}

void switchon_L1(){
  sender.setProtocol(1);
   //sender.setPulseLength(650);
  //sender.sendTriState("0FFFFF0FFFFF");
  sender.sendTriState("0FFFFF0FFFFF");
  delay(10);
  sender.sendTriState("0FFFFF0FFFFF");
  delay(10);
  sender.sendTriState("0FFFFF0FFFFF");
  delay(10);
  sender.sendTriState("0FFFFF0FFFFF");
  //delay(1000);
  //sender.sendTriState("0FFFFF0FFFFF");
  //delay(10);
  Serial.println("ON2 pressed");
}

void switchoff_L1(){
  sender.setProtocol(1);
  //sender.setPulseLength(650);
  //sender.sendTriState("0FFFFF0FFFF0");
  sender.sendTriState("0FFFFF0FFFF0");
  delay(10);
  sender.sendTriState("0FFFFF0FFFF0");
  delay(10);
  sender.sendTriState("0FFFFF0FFFF0");
  delay(10);
  sender.sendTriState("0FFFFF0FFFF0");
  //delay(1000);
  //sender.sendTriState("0FFFFF0FFFF0");
  //delay(10);
  Serial.println("OFF2 pressed");
}


void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect("BookshelfESP", user, passw)) {
  Serial.println("connected");
  // ... and subscribe to topic
  client.subscribe("home/bedroom/bookshelf/switch");
  client.subscribe("home/bedroom/bookshelf/brightness");
  client.subscribe("home/bedroom/switch3");
  client.subscribe("home/bedroom/switch4");
  client.subscribe("home/bedroom/lamp1");
  
 } else {
  Serial.print("failed, rc=");
  Serial.print(client.state());
  Serial.println(" try again in 5 seconds");
  // Wait 5 seconds before retrying
  delay(5000);
  }
 }
}


void setup() {
  Serial.begin(9600);
  
  pinMode(led, OUTPUT); 
  
  WiFi.hostname("BookshelfESP");

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  sender.enableTransmit(4);  // An Pin D1

  sender.setProtocol(1);
  sender.setPulseLength(425);

}

void loop() {

 //digitalWrite(led, brightness);

 if (!client.connected()){
  reconnect();
 }
 client.loop();

}
