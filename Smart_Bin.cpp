// This example shows various featues of the library for LCD with 16 chars and 2 lines.

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <LiquidCrystal_PCF8574.h>
#include "ThingSpeak.h"

#define gsm_g Serial2


LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display


char ssid[] = "KAthiR";
char ssid1[] = "Project";
char pass[] = "1234567890";


unsigned long channelID = 2233318;
const char* writeAPIKey = "H2CDNVZ2C4797G0G";  // write API key for your ThingSpeak Channel
const char* server = "api.thingspeak.com";


String latout = "12.751722619778583", lonout = "80.20331983399791";  //12.751722619778583, 80.20331983399791
String smsmsg, gpsval, text;
String usernum = "09080593449";
// String usernum = "08144086513";


int tup = 20;

int timebefore = 0;

#define t1 26
#define e1 25

int distance;
int bin_height = 27, sens_min = 2, balert = 75;
int dis_per;
int dis_t = 250;
int disp = dis_t;
int alert_Status;


#define splash splash1


WiFiClient client;


void setup() {

  int error;

  Serial.begin(115200);
  gsm_g.begin(9600);
  Serial.println("LCD...");
  LcDSet();
  delay(2000);
  lcd.clear();
  setupWifi();
  Serial.print("Connecting to ThingSpeak");
  ThingSpeak.begin(client);
  ArduinoOTA.setHostname("Smart-Bin-2023");
  OTA();
  // splash(0, "Initializing");
  // splash(1, "GSM");
  // SendSmS(usernum, "GSM Initialized!!!", "", "Sending SMS.........");

  delay(2000);


}  // setup()1

void LcDSet() {

  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.clear();

  lcd.home();
  lcd.setBacklight(1);
  splash(0, "IoT Based");
  splash(1, "Smart Bin");
  delay(1000);
}


void setupWifi() {

  Serial.println();
  Serial.println();
  Serial.print(ssid1);
  WiFi.begin(ssid1, pass);

  Serial.print("Connecting to Wifi ");

  delay(2000);

  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print("Connected !!");
    delay(1000);

    delay(1000);
    lcd.clear();


  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error !!");
    delay(1000);

    delay(1000);
    lcd.clear();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {



  if (millis() - timebefore >= 200) {

    timebefore = millis();

    distance = getCm(t1, e1);
    if (distance >= bin_height) {
      distance = bin_height;
    } else if (distance <= sens_min) {
      distance = sens_min;
    }

    dis_per = map(distance, sens_min, bin_height, 100, 0);
    if (dis_per > balert) {
      alert("Bin No 2 Filled at ");
    }

    lcd.setCursor(0, 0);
    lcd.print("Bin LvL :       ");
    lcd.setCursor(10, 0);
    lcd.print(dis_per);
    lcd.print("%");

    tup++;
    
  }


  UpdateServer();
  delay(40);
}


void UpdateServer() {

  if (tup >= 150) {
    if (client.connect(server, 80)) {

      ThingSpeak.setField(1, String(dis_per));

      // write to the ThingSpeak channel
      int x = ThingSpeak.writeFields(channelID, writeAPIKey);
      if (x == 200) {
        Serial.println("Channel update successful.");
        tup = 0;

      } else {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
    }
  }
}


void alert(String msgg) {
  if (alert_Status == 0) {

    gpsval = "  https://maps.google.com/maps?q=";
    gpsval += latout;
    gpsval += ",";
    gpsval += lonout;
    smsmsg = "Emergency ";
    smsmsg += msgg;
    Serial.println(smsmsg + gpsval);
    SendSmS(usernum, smsmsg, gpsval, "Sending SMS.........");
    alert_Status = 1;
  }
}