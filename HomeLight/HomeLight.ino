#include <Adafruit_NeoPixel.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const byte StripPin = D6;
const int StripSegments = 100;

const char* WiFiSSid = "HomeLight";
const char* WiFiPassword = "HL89345211aP";

ESP8266WebServer WebServer(80);

Adafruit_NeoPixel Strip =
  Adafruit_NeoPixel(StripSegments, StripPin, NEO_BRG + NEO_KHZ800);

void SetColor(uint32_t color)
{
  for (uint16_t i = 0; i < Strip.numPixels(); ++i)
  {
    Strip.setPixelColor(i, color);
  }

  Strip.show();
}

void HandleRoot()
{
  WebServer.send(200, "text/html", "<h1>HomeLight - connected</h1>");
}

void HandleColor()
{
  String rStr = WebServer.arg("r");
  String gStr = WebServer.arg("g");
  String bStr = WebServer.arg("b");

  byte r = atoi(rStr.c_str());
  byte g = atoi(gStr.c_str());
  byte b = atoi(bStr.c_str());

  String response = "r = ";
  response += rStr;
  response += " | ";
  response += String(r);

  response += "\ng = ";
  response += gStr;
  response += " | ";
  response += String(g);

  response += "\nb = ";
  response += bStr;
  response += " | ";
  response += String(b);

  SetColor(Strip.Color(r, g, b));

  WebServer.send(200, "text/plain", response);
}

void StartWebServer()
{
  WiFi.softAP(WiFiSSid, WiFiPassword);

  WebServer.on("/", HandleRoot);
  WebServer.on("/color", HTTPMethod::HTTP_POST, HandleColor);
  WebServer.begin();
}

void setup()
{
  StartWebServer();

  Strip.begin();
  Strip.show();
}

void loop()
{
  WebServer.handleClient();
}
