#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "child";
const char* password = "childchild";

const int ledPin = 2; 

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  //initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS initialization failed!");
    return;
  }
  //Mount SPIFFS filesystem
  if(!SPIFFS.begin()){
    Serial.println("Failed to mount SPIFFS filesystem!");
  }
  
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP address: ");
  Serial.println(IP);

  //Serve the image from SPIFFS
  server.on("/image.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/image.jpg", "image/jpeg");
  });

  //Serve the HTML page with JavaScript function to toggle LED and image
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, [](const String& var){
      if(var == "LED_STATE"){
        return digitalRead(ledPin) == HIGH ? "ON" : "OFF";
      }
      else if(var == "LED_IMAGE"){
        return "/image.jpg";
      }
      return String();
    });
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<!DOCTYPE html><html><head><title>ESP32 Web Server</title></head><body><h1  style='font-size: 100px; text-align: auto'>ESP32 Web Server</h1><button onclick=\"toggleLED('/on')\" style='font-size: 70px; text-align: auto;'>Turn On LED</button><button onclick=\"toggleLED('/off')\" style='font-size: 70px; text-align: auto;'>Turn Off LED</button><script>function toggleLED(url) { var xhr = new XMLHttpRequest(); xhr.open('GET', url, true); xhr.send(); }</script></body></html>");
  });

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);
    request->send(200);
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);
    request->send(200);
  });

  server.begin();
}

void loop() {
  // Nothing to do here, AsyncWebServer handles requests asynchronously
}
