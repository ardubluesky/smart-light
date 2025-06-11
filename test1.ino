#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Wire.h>
#include <RTClib.h>

// Replace with your WiFi credentials
const char* ssid = "Akash";
const char* password = "3937343031";

// Timezone offset in seconds (for IST = UTC+5:30 = 19800 seconds)
const long utcOffsetInSeconds = 19800;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
RTC_DS3231 rtc;

const int ledPin = LED_BUILTIN;

// Desired LED ON and OFF times in epoch (UNIX) time
unsigned long ledOnEpoch = 0;
unsigned long ledOffEpoch = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH); // LED off (on NodeMCU, LOW = ON)

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  timeClient.begin();
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time from NTP");
    rtc.adjust(DateTime(timeClient.getEpochTime()));
  }

  // For today's date, build epoch times for 11:14 and 11:17
  DateTime now = rtc.now();
  DateTime ledOnTime(now.year(), now.month(), now.day(), 11, 16, 0);
  DateTime ledOffTime(now.year(), now.month(), now.day(), 11, 18, 0);

  ledOnEpoch = ledOnTime.unixtime();
  ledOffEpoch = ledOffTime.unixtime();

  Serial.print("LED ON at: "); Serial.println(ledOnEpoch);
  Serial.print("LED OFF at: "); Serial.println(ledOffEpoch);
}

void loop() {
  DateTime now = rtc.now();
  unsigned long currentEpoch = now.unixtime();

  if (currentEpoch >= ledOnEpoch && currentEpoch < ledOffEpoch) {
    digitalWrite(ledPin, LOW); // Turn ON LED
  } else {
    digitalWrite(ledPin, HIGH); // Turn OFF LED
  }

  // Optional debug output
  Serial.print("Time: "); Serial.println(now.timestamp());
  delay(1000); // Check every second
}
