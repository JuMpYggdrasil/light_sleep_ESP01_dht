#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define USE_SERIAL_DEBUG false

// Required for LIGHT_SLEEP_T delay mode
extern "C" {
#include "user_interface.h"
}

const char* ssid = "JUMP";
const char* password = "025260652";
const char* mqtt_server = "siriprapawat.trueddns.com";

#define DHT11PIN 2                      // define the digital I/O pin
#define DHT11TYPE DHT11                 // DHT 11 
DHT dht(DHT11PIN, DHT11TYPE);

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


void setup() {                          //The setup function is called once at startup of the sketch
  pinMode(BUILTIN_LED, OUTPUT);         // Initialize the BUILTIN_LED pin as an output
#if USE_SERIAL_DEBUG
  Serial.begin(115200);
  while (!Serial) { }

  Serial.println();
  Serial.println("Start device in normal mode!");

#endif
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 14283); //default 1883
  client.setCallback(callbackMqtt);
}


void loop() {

#if USE_SERIAL_DEBUG
  Serial.println(F("--"));
  Serial.println("Enter light sleep mode");
  Serial.println(F("|"));
  Serial.println(F("|"));
#endif

  // Here all the code to put con light sleep
  // the problem is that there is a bug on this
  // process
  //wifi_station_disconnect();          //not needed
  uint32_t sleep_time_in_ms = 10000;
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(callbackWakeup);
  wifi_fpm_do_sleep(sleep_time_in_ms * 1000 );
  delay(sleep_time_in_ms + 1);

#if USE_SERIAL_DEBUG
  Serial.println("Exit light sleep mode");
  Serial.println(F("--"));
  Serial.println();
#endif

  setup_wifi();

  wifi_set_sleep_type(NONE_SLEEP_T);
  delay(1);

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

#if USE_SERIAL_DEBUG
  Serial.println("Send MQTT data");
#endif

  float t = dht.readTemperature();
  if (isnan(t)) {
    delay(500);
    float t = dht.readTemperature();
    if (!isnan(t)) {
      snprintf (msg, MSG_BUFFER_SIZE, "%f", t);
      client.publish("temp", msg);
    }
  } else {
    snprintf (msg, MSG_BUFFER_SIZE, "%f", t);
    client.publish("temp", msg);
  }

  float h = dht.readHumidity();
  if (isnan(h)) {
    delay(500);
    float h = dht.readHumidity();
    if (!isnan(h)) {
      snprintf (msg, MSG_BUFFER_SIZE, "%f", h);
      client.publish("humid", msg);
      delay(100);//wait mqtt send complete
    }
  } else {
    snprintf (msg, MSG_BUFFER_SIZE, "%f", h);
    client.publish("humid", msg);
    delay(100);//wait mqtt send complete
  }
}

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#if USE_SERIAL_DEBUG
    Serial.print(".");
#endif
  }

#if USE_SERIAL_DEBUG
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif

}

void callbackWakeup() {
#if USE_SERIAL_DEBUG
  Serial.println("Callback");
  Serial.flush();
#endif

}
void callbackMqtt(char* topic, byte* payload, unsigned int length) {
#if USE_SERIAL_DEBUG
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
#endif

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
#if USE_SERIAL_DEBUG
    Serial.print("Attempting MQTT connection...");
#endif
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
#if USE_SERIAL_DEBUG
      Serial.println("connected");
#endif
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
#if USE_SERIAL_DEBUG
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
#endif
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
