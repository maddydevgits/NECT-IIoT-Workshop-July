#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#define LED D0

DHT dht(D2,DHT11);
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

const char* ssid = "admin";
const char* password =  "12345678";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
void setup() 
{
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(MQTTcallback);
  while (!client.connected()) 
  {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
  client.subscribe("nec/madhu");
}
void MQTTcallback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message received in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String message;
  for (int i = 0; i < length; i++) 
  {
    message = message + (char)payload[i];
  }
  Serial.print(message);
  if (message == "on") 
  {
    digitalWrite(LED, HIGH);
  }
  else if (message == "off") 
  {
    digitalWrite(LED, LOW);
  }
  Serial.println();
  Serial.println("-----------------------");
}
void loop() 
{
  float h=dht.readHumidity();
  float t=dht.readTemperature();
  if(isnan(h)||isnan(t))
    return;
  snprintf (msg, MSG_BUFFER_SIZE, "H: %f, T: %f", h,t);
  client.publish("nec/dht11", msg);
  delay(1000);
  client.loop();
}
