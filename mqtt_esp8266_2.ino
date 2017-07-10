/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "FS.h"


// Update these with values suitable for your network.

const char* mqtt_server = "mqtt.thingspeak.com";
const char* outTopic = "channels/300045/publish/G8BXC6E9SQ4LT6GX";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];



void setup_wifi(const char* ssid, const char *password) {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  char buffer[80];

  Serial.begin(115200);
  delay(500);
  Serial.println("ESP-12E online");
  
  DynamicJsonBuffer jsonBuffer(80);
 
  SPIFFS.begin();
  File f = SPIFFS.open("config.json","r");  // This file (stored on SPIFFS) contains WiFi SSID and password

  if(f){
    f.readBytes(buffer,80);  
    JsonObject& o = jsonBuffer.parseObject(buffer);  // decode config.json file contents into object
    setup_wifi(o["SSID"],o["Key"]);   // Initialise WiFi with parameters stored in SPIFFS memory
    f.close(); 
  }
  client.setServer(mqtt_server, 1883);
}




void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    }
  }
}


// Class to check if the Slot detector has seen an object go through the sensor
class Slot {
  bool lastState;
  long counter;
  bool changed;
public:
  Slot(){
    lastState = changed = false;
    counter = 0;  
  }
  void recordValue(bool updown);
  bool isChanged(void);
  long getCount(void);
};

// Record change in state of sensor 
void Slot::recordValue(bool updown){
   if(updown != lastState){
     if(!updown){  // changing to off
        changed = true;
        ++counter;
     }
     lastState = updown;
   }
}

// Has the sensor seen a transition recently/
bool Slot::isChanged(void){
  return changed;
}

// Count number of times the beam has been broken
long Slot::getCount(void){
  changed = false;
  return counter;
}


Slot slot;  // slot sensor object instance


void loop() {

  int d0 = digitalRead(0);
  slot.recordValue(d0);

  if(slot.isChanged()){    // when there's a change we want to send a value to Thingspeak

    snprintf (msg, 50, "field1=%ld", slot.getCount() );
    
    if (!client.connected()) {
      reconnect();
    }  
    client.publish(outTopic, msg);
         
    Serial.print("Publish message: ");
    Serial.println(msg);        
  }

  client.loop();
  
}

  
