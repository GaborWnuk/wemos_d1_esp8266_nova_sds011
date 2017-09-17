#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_BMP085.h>
#include <SDS011.h>
#include "base64.hpp"
#include "RunningAverage.h"

// Time to sleep (in seconds):
const int sleepTime = 60 * 1;

const char* ssid = "XXXXXXX";
const char* password = "XXXXXXXX

const char* host = "192.168.1.106";
const int httpPort = 8086;

const char* influxUser = "XXXXX";
const char* influxPassword = "XXXXXXXX";

float p10, p25;
int error;
SDS011 sds;

RunningAverage pm25Stats(10); 
RunningAverage pm10Stats(10); 
RunningAverage temperatureStats(10); 
RunningAverage pressureStats(10); 

Adafruit_BMP085 bmp;

void reconnect() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void http_post(String data) {
  Serial.print("connecting to ");
  Serial.println(host);

    // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/write?db=iot";
  Serial.print("requesting URL: ");

  String auth = String(influxUser) + ":" + String(influxPassword);
  unsigned char authChar[32];
  auth.toCharArray(( char*)authChar, auth.length()+1);
  unsigned char encodedAuth[64];
  encode_base64(authChar, auth.length(), encodedAuth);

  String body = String("POST ") + url + " HTTP/1.0\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Basic " + String(( char*)encodedAuth) + "\r\n" +
               "Content-Length: " + data.length() + "\r\n"
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "\r\n" +
               data;

  Serial.print("body: ");
  Serial.println(body);
  client.print(body);

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  Serial.println("closing connection");
}

void submit_air_conditions(String pm10, String pm25) {
  http_post("air_quality,sensor=pm10,type=inhouse value=" + pm10 + "\nair_quality,sensor=pm2.5,type=inhouse value=" + pm25);
}

void submit_environment_conditions(String temperature, String pressure) {
  http_post("environment,sensor=temperature,type=inhouse value=" + temperature + "\nenvironment,sensor=pressure,type=inhouse value=" + pressure);
}


void setup() {
  Serial.begin(115200);
  sds.begin(D6, D7);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
}

void loop() {
  pm25Stats.clear();
  pm10Stats.clear();
  temperatureStats.clear();
  pressureStats.clear();
  
  reconnect();

  sds.wakeup();
  Serial.println("Calibrating SDS011 (15 sec)");
  delay(15000);

  for (int i = 0; i < 10; i++)
  {
    error = sds.read(&p25, &p10);

    if (!error) {
      pm25Stats.addValue(p25); 
      pm10Stats.addValue(p10);
    }
    
    temperatureStats.addValue(bmp.readTemperature());
    pressureStats.addValue(bmp.readPressure());

    Serial.println("Average PM10: " + String(pm10Stats.getAverage()) + ", PM2.5: "+ String(pm25Stats.getAverage()) + 
                    ", temperature: " + String(temperatureStats.getAverage()) + ", pressure: " + String(pressureStats.getAverage() / 100.0));
    delay(1500);
  }

  submit_air_conditions(String(pm10Stats.getAverage()), String(pm25Stats.getAverage()));
  submit_environment_conditions(String(temperatureStats.getAverage()), String(pressureStats.getAverage() / 100.0));

  Serial.println("Sleep(" + String(sleepTime) + ")");
  sds.sleep();
  delay(sleepTime * 1000);
}
