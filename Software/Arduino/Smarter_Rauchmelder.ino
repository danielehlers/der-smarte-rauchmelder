//---Include libraries------------------------------

#include <FastLED.h>
#include <PubSubClient.h>
#include <WiFiManager.h>  

//--------------------------------------------------------

#define MQ2pin (A0)
#define NUM_LEDS 4
#define DATA_PIN D6 

//--------------------------------------------------------

CRGB leds[NUM_LEDS];
WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient client(espClient);

//--------------------------------------------------------

const int buzzer = D2;
const int button = D3;

const char* mqtt_topic_publish = "Rauchmelder";
const char* mqtt_topic_subscribe = "Rauchmelder";

//--------------------------------------------------------


//---Do not change the following four parameters----------

const char* mqtt_server = "mqtt.iot.informatik.uni-oldenburg.de";
const int mqtt_port = 2883;
const char* mqtt_user = "sutk";
const char* mqtt_pw = "SoftSkills";

//--------------------------------------------------------

long lastMsg = 0;
int val = 0;

// Variables for the switch function 
int buttonstatus = 0;
int buttonZaehler = 0;
int letzterStatus = 0;

//Variable to store sensor value
float sensorValue;  

//--------------------------------------------------------

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) { 
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID: Client ID MUSS inviduell sein, da der MQTT Broker nicht mehrere Clients mit derselben ID bedienen kann
    String clientId = "Client-"; 
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pw)) { // Attempt to connect
      Serial.println("connected");    
      client.subscribe(mqtt_topic_subscribe); // Once connected, publish an announcement
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

//--------------------------------------------------------

void setup() {
  Serial.begin(115200); // Sets the serial port to 115200
  wifiManager.autoConnect("Smarter Rauchmelder");
  client.setServer(mqtt_server, mqtt_port);
  Serial.println("Gas sensor warming up!"); 
  delay(20000); // Allows the MQ-2 to warm up

  // FastLED setup
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); 
  FastLED.setBrightness(255);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 400);
  for (int i = 1; i < NUM_LEDS; i++) {
    leds[i] = CRGB( 255, 0, 0);
  }
  FastLED.show();
  delay(200);
  for (int i = 1; i < NUM_LEDS; i++) {
    leds[i] = CRGB( 0, 0, 0);
  }
  FastLED.show();
  FastLED.clear();
  delay(30);
  
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);
}

//--------------------------------------------------------

void loop() {
  leds[0] = CRGB::Black;
  buttonstatus = digitalRead(button);
  sensorValue = analogRead(MQ2pin); // Read analog input pin 0
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if(buttonstatus != letzterStatus) {
    if (buttonstatus == 1) {
      buttonZaehler++;
    } else {
      digitalWrite(buzzer, LOW); 
    }
  }
  
  if (buttonZaehler % 2 == 0) {
    // To check on the serial monitor
    Serial.print("Sensor Value: "); 
    Serial.print(sensorValue);
    
    if(sensorValue > 300) {  // Change this value for easier testing
      client.publish(mqtt_topic_publish, "Es brennt in der Wohnung!");
      Serial.print(" | Smoke detected!"); // To check on the serial monitor
      Alarm();
      LED_effect_Alarm();
      Alarm();
    }
    buttonZaehler = 0;
  } else {
    digitalWrite(buzzer, LOW);
  }
  
  // To check on the serial monitor
  letzterStatus = buttonstatus; 
  Serial.println(buttonZaehler);
   
  delay(2000);
}

//--------------------------------------------------------

void LED_effect_Alarm(){
    FastLED.setBrightness(255);
    
    for (int i = 1; i < NUM_LEDS; i++) {
    leds[i] = CRGB( 255, 0, 0);
    }
    FastLED.show();
    delay(500);  
    
    for (int i = 1; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }    
    FastLED.show();
    delay(500);
    FastLED.setBrightness(255); 
      
    for (int i = 1; i < NUM_LEDS; i++) {
    leds[i] = CRGB( 255, 0, 0);
    }  
    FastLED.show();
    delay(500); 
       
    for (int i = 1; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Black;
    }    
    FastLED.show();
    delay(750);
}

//--------------------------------------------------------

void Alarm() {
   digitalWrite(buzzer, HIGH);
   delay(500);
   digitalWrite(buzzer, LOW);
   delay(250);
   digitalWrite(buzzer, HIGH);
   delay(500);
   digitalWrite(buzzer, LOW);  
}
  
