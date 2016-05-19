#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "FS.h"

void Blink(int t1, int t2);
void ShowError(int t1, int t2);
void SetColor(uint32_t color);

void StartWiFiAccessPoint();
void ConnectWiFi(String ssid, String Password);

void HandleGetWiFi();
void HandlePostWiFi();
void HandleRoot();
void HandlePostColor();
String GetWiFiConfigurationPage();

bool ReadWiFiConfiguration(String& ssid, String& password);
void WriteWiFiConfiguration(const String& ssid, const String& password);

void ReadEEPROM(byte* buffer, unsigned int start, unsigned int count);
void WriteEEPROM(const byte* buffer, unsigned int start, unsigned int count);

const byte LedPin = D0;
const byte StripPin = D6;
const int StripSegments = 100;

const char* WiFiSSid = "HomeLight";
const char* WiFiPassword = "45724a31";

ESP8266WebServer WebServer(80);
Adafruit_NeoPixel Strip = Adafruit_NeoPixel(StripSegments, StripPin, NEO_BRG + NEO_KHZ800);

void setup()
{
    pinMode(LedPin, OUTPUT);

    Blink(500, 100);
    Blink(500, 100);

    String ssid;
    String password;
    if (ReadWiFiConfiguration(ssid, password))
    {
        digitalWrite(LedPin, HIGH); // off
        Strip.begin();
        Strip.show();
        ConnectWiFi(ssid, password);
    }
    else
    {
        digitalWrite(LedPin, LOW); // on
        StartWiFiAccessPoint();
    }
}

void loop()
{
    //WebServer.handleClient();
}


void Blink(int t1, int t2)
{
    digitalWrite(LedPin, LOW);
    delay(t1);
    digitalWrite(LedPin, HIGH);
    delay(t2);
}

void ShowError(int t1, int t2)
{
    while (true)
    {
        Blink(t1, t2);
    }
}

void SetColor(uint32_t color)
{
    for (uint16_t i = 0; i < Strip.numPixels(); ++i)
    {
        Strip.setPixelColor(i, color);
    }

    Strip.show();
}


void StartWiFiAccessPoint()
{
    WiFi.softAP(WiFiSSid, WiFiPassword);

    WebServer.on("/", HandleGetWiFi);
    WebServer.on("/", HTTPMethod::HTTP_POST, HandlePostWiFi);
    WebServer.begin();
}

void ConnectWiFi(String ssid, String password)
{
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        Blink(200, 200);
        delay(10);
    }

    WebServer.on("/", HandleRoot);
    WebServer.on("/color", HTTPMethod::HTTP_POST, HandlePostColor);
    WebServer.begin();
}


void HandleGetWiFi()
{
    String page = GetWiFiConfigurationPage();
    WebServer.send(200, "text/html", page);
}

void HandlePostWiFi()
{
    // TODO
    
    //WebServer.send(200, "text/plain", "OK");
    //ESP.restart();
}

void HandleRoot()
{
    WebServer.send(200, "text/plain", "OK");
}

void HandlePostColor()
{
    String rStr = WebServer.arg("r");
    String gStr = WebServer.arg("g");
    String bStr = WebServer.arg("b");

    byte r = atoi(rStr.c_str());
    byte g = atoi(gStr.c_str());
    byte b = atoi(bStr.c_str());

    SetColor(Strip.Color(r, g, b));

    WebServer.send(200);
}

String GetWiFiConfigurationPage()
{
    if (!SPIFFS.begin())
    {
        ShowError(400, 100);
        return "";
    }

    File file = SPIFFS.open("/WiFiConfiguration.html", "r");
    if (!file)
    {
        ShowError(500, 100);
        return "";
    }

    return file.readString();
}


bool ReadWiFiConfiguration(String& ssid, String& password)
{
    EEPROM.begin(512);

    byte flag = EEPROM.read(0);
    if (flag != 1)
    {
        EEPROM.end();
        return false;
    }

    byte buffer[128];

    ReadEEPROM(buffer, 1, 128);
    ssid = String((char*)buffer);

    ReadEEPROM(buffer, 130, 128);
    password = String((char*)buffer);

    EEPROM.end();
}

void WriteWiFiConfiguration(const String& ssid, const String& password)
{
    EEPROM.begin(512);

    EEPROM.write(0, 1);

    byte buffer[128] = { 0 };

    ssid.getBytes(buffer, 128);
    WriteEEPROM(buffer, 1, ssid.length());

    password.getBytes(buffer, 128);
    WriteEEPROM(buffer, 130, password.length());

    EEPROM.end();
}


void ReadEEPROM(byte* buffer, unsigned int start, unsigned int count)
{
    for (int i = 0; i < count; ++i)
    {
        buffer[i] = EEPROM.read(i + start);
    }
}

void WriteEEPROM(const byte* buffer, unsigned int start, unsigned int count)
{
    for (int i = 0; i < count; ++i)
    {
        EEPROM.write(i + start, buffer[i]);
    }
}
