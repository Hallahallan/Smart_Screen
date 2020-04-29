#include "Arduino_ST7789.h"
#include <Adafruit_mfGFX.h>
#include <RTClib.h>

#include <SPI.h>

#define TFT_MOSI A5
#define TFT_MISO A4
#define TFT_CLK A3
#define TFT_CS A2
#define TFT_RST A1
#define TFT_DC A0

#define DHTPIN 2
#define DHTTYPE DHT11

Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_CS);

const int buttonPin = D0;
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
String temp;
String frontDisplayString;
String secondFrontDisplayString;
String estimatedTimeString;
String weatherStackString;
String currentTime;
String currentHour;
String currentMinute;
String currentSecond;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);

  Time.zone(+2); // Defining Local Time zone for Oslo

  // Initializing screen
  tft.init(135, 240);
  tft.setRotation(3);
  tft.fillScreen(BLACK);

  // Subscriptions to webhooks
  Particle.subscribe("hook-response/FrontDisplay", receiveFrontDisplay,
                     MY_DEVICES);
  Particle.subscribe("hook-response/SecondFrontDisplay",
                     receiveSecondFrontDisplay, MY_DEVICES);
  Particle.subscribe("hook-response/EstimatedTime", receiveETA, MY_DEVICES);
  Particle.subscribe("hook-response/WeatherStack", receiveWeatherStack,
                     MY_DEVICES);
}

void loop() {

  // Set time
  currentHour = Time.hour();
  currentMinute = Time.minute();
  currentSecond = Time.second();
  currentTime = currentHour + ":" + currentMinute + ":" + currentSecond;

  // Get data from webhooks
  Particle.publish("FrontDisplay", "", PRIVATE);
  Particle.publish("SecondFrontDisplay", "", PRIVATE);
  Particle.publish("EstimatedTime", "", PRIVATE);
  Particle.publish("WeatherStack", "", PRIVATE);

  // Button press activates and renders info on screen
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      printData();
    }
  }
  lastButtonState = reading;
}

// Recievings from Webhooks
void receiveFrontDisplay(const char *event, const char *data) {
  frontDisplayString = data;
}

void receiveSecondFrontDisplay(const char *event, const char *data) {
  secondFrontDisplayString = data;
}

void receiveETA(const char *event, const char *data) {
  estimatedTimeString = data;
}

void receiveWeatherStack(const char *event, const char *data) { temp = data; }

// Printing data to screen here
void printData() {

  tft.fillScreen(BLACK);
  tft.setFont(ARIAL_8);

  tft.setTextColor(YELLOW);
  tft.setCursor(190, 5);
  tft.setTextSize(2);
  tft.print(String(temp));

  // Printing degrees Celsius in a hacky way since degree symbol isnt supported
  // in ASCII
  tft.setCursor(225, 5);
  tft.print("C");
  tft.setCursor(218, 3);
  tft.setTextSize(1);
  tft.print("o");

  // Clock
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.setCursor(1, 1);
  tft.print("Current Time:");
  tft.setTextSize(3);
  tft.setCursor(1, 15);
  tft.print(currentTime);

  // Bus stop text
  tft.setTextColor(WHITE);
  tft.setCursor(0, 50);
  tft.setTextSize(2);
  tft.print("Telthusbakken");

  /*******************0**************/
  // Number of bus
  tft.setTextColor(RED);
  tft.setCursor(0, 75);
  tft.setTextSize(2);
  tft.print(estimatedTimeString[9]);
  tft.print(estimatedTimeString[10]);

  // Name and display of bus
  tft.setTextColor(WHITE);
  tft.setCursor(0, 92);
  tft.setTextSize(1);
  tft.print(frontDisplayString);

  // ETA of bus
  tft.setCursor(140, 75);
  tft.setTextSize(2);
  tft.print(estimatedTimeString[33]);
  tft.print(estimatedTimeString[34]);
  tft.print(estimatedTimeString[35]);
  tft.print(estimatedTimeString[36]);
  tft.print(estimatedTimeString[37]);
  tft.print(estimatedTimeString[38]);
  tft.print(estimatedTimeString[39]);
  tft.print(estimatedTimeString[40]);

  // Number of bus 1
  tft.setTextColor(RED);
  tft.setCursor(0, 105);
  tft.setTextSize(2);
  tft.print(estimatedTimeString[20]);
  tft.print(estimatedTimeString[21]);

  // Name and display of bus 1
  tft.setTextColor(WHITE);
  tft.setCursor(0, 122);
  tft.setTextSize(1);
  tft.print(secondFrontDisplayString);

  // ETA of bus 1
  tft.setCursor(140, 105);
  tft.setTextSize(2);
  tft.print(estimatedTimeString[57]);
  tft.print(estimatedTimeString[58]);
  tft.print(estimatedTimeString[59]);
  tft.print(estimatedTimeString[60]);
  tft.print(estimatedTimeString[61]);
  tft.print(estimatedTimeString[62]);
  tft.print(estimatedTimeString[63]);
  tft.print(estimatedTimeString[64]);

  delay(5000);
  tft.fillScreen(BLACK);
}
