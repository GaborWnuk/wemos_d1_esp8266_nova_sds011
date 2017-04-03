#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_BMP085.h>
#include <SDS011.h>

// Time to sleep (in seconds):
const int sleepTime = 60 * 5;

const char* ssid = "IoTHUB";
const char* password = "YOURWIFIPASSWORD";

const char* host = "192.168.10.1";
const int httpPort = 8086;

float p10, p25;
int error;
SDS011 sds;

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
  Serial.println(url);

  String body = String("POST ") + url + " HTTP/1.0\r\n" +
               "Host: " + host + "\r\n" +
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
  http_post("air_quality,sensor=pm10 value=" + pm10 + "\nair_quality,sensor=pm2.5 value=" + pm25);
}

void submit_environment_conditions(String temperature, String pressure) {
  http_post("environment,sensor=temperature value=" + temperature + "\nenvironment,sensor=pressure value=" + pressure);
}

void setup() {
  Serial.begin(115200);
  sds.begin(D6, D7);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }
}

void loop() {
  reconnect();

  sds.wakeup();
  Serial.println("Calibrating SDS011 (10 sec)");
  delay(20000);
  error = sds.read(&p25, &p10);

  if (!error) {
    submit_air_conditions(String(p10), String(p25));
  }

  submit_environment_conditions(String(bmp.readTemperature()), String(bmp.readPressure() / 100.0));

  Serial.println("Sleep(" + String(sleepTime) + ")");
  sds.sleep();
  delay(sleepTime * 1000);
}
