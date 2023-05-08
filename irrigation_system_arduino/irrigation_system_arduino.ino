
#include <ESP8266WiFi.h>
#include <SimpleDHT.h>

String apiKey = "XWKTI4DAKWK03B9F";
const char* ssid = "Valladares";
const char* password = "Rodrigo1997";
const char* server = "api.thingspeak.com";

int pinDHT11 = D0;
int motorPin = D3;
SimpleDHT11 dht11;
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);
  
  pinMode(A0, INPUT);
  pinMode(motorPin, OUTPUT);

  WiFi.begin(ssid, password);
  
  Serial.println();
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
}

void loop() {

  byte temperaturaAmbiente = 0;
  byte humedadAmbiente = 0;
  
  float humedadSuelo = analogRead(A0);
  int porcentajeHumedadSuelo = map(humedadSuelo, 0, 1023, 100, 0);

  if (dht11.read(pinDHT11, &temperaturaAmbiente, &humedadAmbiente, NULL)) {
    Serial.println("Read DHT11 failed.");
    return;
  }
  
  if (porcentajeHumedadSuelo < 40) { 
    digitalWrite(motorPin, LOW);   
  } else if (porcentajeHumedadSuelo < 60 && temperaturaAmbiente > 30) {  
    digitalWrite(motorPin, LOW);    
  } else {
    digitalWrite(motorPin, HIGH); 
  }

  if (client.connect(server,80)) {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String((int)temperaturaAmbiente);
    postStr +="&field2=";
    postStr += String((int)humedadAmbiente);
    postStr +="&field3=";
    postStr += String(porcentajeHumedadSuelo);
    postStr += "\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("% send to Thingspeak");
  }
  
  client.stop();
  
  Serial.println("Waiting…");

  delay(10000);
}