#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <U8g2lib.h>
#include <Wire.h> // Include Wire if you're using I2C

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

#include "secrets.h"

#define DHTPIN 27

DHT dht(DHTPIN, DHT11);

//initialize the OLED display
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 22, /* data=*/ 21, /* reset=*/ U8X8_PIN_NONE);


//WebServer  instance
WebServer server(80);



void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }


  dht.begin();
  u8g2.begin();  //initialize the OLED display

  //Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  
  //Start the WebServer from file system
  const char* filename = "/index.html";
  const char* uri = "/";

  server.serveStatic(uri, LittleFS, filename);

  server.on("/data", HTTP_GET, []() {
    server.send(200, "text/json", "{\"humidity\":"+String(dht.readHumidity())+",\"temperature\":"+String(dht.readTemperature())+"}");
  });

  server.on("/led1", HTTP_GET, []() {
    digitalWrite(2, !digitalRead(2));
    server.send(200, "text/plain", "LED1 is ON");
  });

  server.on("/led2", HTTP_GET, []() {
    digitalWrite(4, !digitalRead(4));
    server.send(200, "text/plain", "LED2 is OFF");
  });

  server.begin();
  
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
}

int counter = 0;
void loop() {
  server.handleClient();

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C ");
   u8g2.firstPage();
  do {
    u8g2.drawHLine(0,0,10);
    u8g2.drawHLine(0,31,10);
  
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,24,"Hello World!");
    u8g2.drawStr(0,16,"Humidity: ");
    u8g2.setCursor(70,16);
    u8g2.print(humidity);
    u8g2.print(" %");
    u8g2.drawStr(0,8,"Temperature: ");
    u8g2.setCursor(70,8);
    u8g2.print(temperature);
    u8g2.print(" C");

    counter++;
    u8g2.setCursor(0,32);
    u8g2.print("Counter: ");
    u8g2.print(counter);
  } while ( u8g2.nextPage() );
}



