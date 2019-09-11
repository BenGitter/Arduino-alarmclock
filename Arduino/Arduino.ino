/* 
 * btn 0: DECREMENT     ~1010 980, 1024
 * btn 1: INCREMENT     ~930  900, 960
 * btn 2: SET_TIME      ~840  810, 870
 * btn 3: SET_ALARM     ~696  660, 730
 * btn 4: SWITCH_ALARM  ~510  480, 540
 */

#define DECREMENT_ANALOG 1010
#define INCREMENT_ANALOG 930
#define SET_TIME_ANALOG 840
#define SET_ALARM_ANALOG 695
#define SWITCH_ALARM_ANALOG 510
#define NONE_MARGIN 100
#define ANALOG_MARGIN 30

#include "AlarmClock.h"
#include "WiFiEsp.h"
#include "SoftwareSerial.h"

AlarmClock alarmClock;

//SoftwareSerial Serial1(5, 6);
WiFiEspClient client;

char ssid[] = "Fritzbox";
char pass[] = "0180462067";
int status = WL_IDLE_STATUS;
char server[] = "192.168.178.24";

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);
    WiFi.init(&Serial1);

    tryServerTime();
}

enum button btnPressed = NONE;
enum button btnReading = NONE;
enum button prevBtnReading = NONE;
unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 50;

void loop()
{
    alarmClock.tickTimer();

    int btnValue = analogRead(A0);

    if (btnValue > DECREMENT_ANALOG - ANALOG_MARGIN && btnValue < DECREMENT_ANALOG + ANALOG_MARGIN)
        btnReading = DECREMENT;
    else if (btnValue > INCREMENT_ANALOG - ANALOG_MARGIN && btnValue < INCREMENT_ANALOG + ANALOG_MARGIN)
        btnReading = INCREMENT;
    else if (btnValue > SET_TIME_ANALOG - ANALOG_MARGIN && btnValue < SET_TIME_ANALOG + ANALOG_MARGIN)
        btnReading = SET_TIME;
    else if (btnValue > SET_ALARM_ANALOG - ANALOG_MARGIN && btnValue < SET_ALARM_ANALOG + ANALOG_MARGIN)
        btnReading = SET_ALARM;
    else if (btnValue > SWITCH_ALARM_ANALOG - ANALOG_MARGIN && btnValue < SWITCH_ALARM_ANALOG + ANALOG_MARGIN)
        btnReading = SWITCH_ALARM;
    else if (btnValue < NONE_MARGIN)
        btnReading = NONE;

    if (btnReading != prevBtnReading)
        lastDebounceTime = millis();

    if ((millis() - lastDebounceTime) > debounceDelay && btnReading != btnPressed)
    {
        btnPressed = btnReading;
        if (btnPressed != NONE)
        {
            Serial.print("Button: ");
            Serial.println(btnPressed);
            alarmClock.handleButtonPress(btnPressed);
        }
    }

    prevBtnReading = btnReading;
}

void tryServerTime()
{
    // check for the presence of the shield
    if (WiFi.status() == WL_NO_SHIELD)
    {
        Serial.println("WiFi shield not present");
        // don't continue
        while (true)
            ;
    }

    // attempt to connect to WiFi network
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network
        status = WiFi.begin(ssid, pass);
    }

    // you're connected now, so print out the data
    Serial.println("You're connected to the network");

    //  printWifiStatus();
    Serial.println();
    Serial.println("Starting connection to server...");
    // if you get a connection, report back via serial
    if (client.connect(server, 3000))
    {
        Serial.println("Connected to server");
        // Make a HTTP request
        client.println("GET /time HTTP/1.1");
        client.println("Host: 192.168.178.24");
        client.println("Cache-Control: no-cache");
        client.println("Connection: close");
        client.println();
    }
    else
    {
        Serial.println("Fetching server time failed.");
        return;
    }

    delay(10);

    String msg;
    while (client.available())
    {
        char c = client.read();
        msg += c;
    }

    int startPosHours = msg.indexOf("<hours>") + 7;
    int endPosHours = msg.indexOf("</hours>", startPosHours);
    int startPosMinutes = msg.indexOf("<minutes>") + 9;
    int endPosMinutes = msg.indexOf("</minutes>", startPosMinutes);

    int hours = msg.substring(startPosHours, endPosHours).toInt();
    int minutes = msg.substring(startPosMinutes, endPosMinutes).toInt();
    Serial.println(hours);
    Serial.println(minutes);
    alarmClock.setTime(hours, minutes);
}

//void printWifiStatus()
//{
//  // print the SSID of the network you're attached to
//  Serial.print("SSID: ");
//  Serial.println(WiFi.SSID());
//
//  // print your WiFi shield's IP address
//  IPAddress ip = WiFi.localIP();
//  Serial.print("IP Address: ");
//  Serial.println(ip);
//
//  // print the received signal strength
//  long rssi = WiFi.RSSI();
//  Serial.print("Signal strength (RSSI):");
//  Serial.print(rssi);
//  Serial.println(" dBm");
//}
