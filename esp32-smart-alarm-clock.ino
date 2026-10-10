#include <WiFi.h>
#include <WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// ---------------- WIFI ----------------
const char* ssid = "YOU_WIFI_NAME/SSID";      // ← Change
const char* password = "YOUR_WIFI_PASSWORD"; // ← Change

// ---------------- RELAY ----------------
const int relayPin = 26;
#define RELAY_ON LOW
#define RELAY_OFF HIGH

// ---------------- SERVER ----------------
WebServer server(80);

// ---------------- TIME ----------------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// ---------------- ALARMS ----------------
#define MAX_ALARMS 5
#define RING_TIME 300000 // 5 min

struct Alarm {
  int hour;
  int minute;
  bool enabled;
  bool active;
  unsigned long startMillis;
};

Alarm alarms[MAX_ALARMS];

// ---------------- STATE ----------------
bool manualMotorState = false; // ONLY from buttons

unsigned long lastCheck = 0;

// ---------------- MOTOR ----------------
void motorOn() {
  digitalWrite(relayPin, RELAY_ON);
  manualMotorState = true;
}

void motorOff() {
  digitalWrite(relayPin, RELAY_OFF);
  manualMotorState = false;
}

// ---------------- HTML ----------------
String page() {
  int h = (timeClient.getHours() + 2) % 24;
  int m = timeClient.getMinutes();

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Alarm Clock</title>

<style>
body { font-family: Arial; background:#111; color:white; text-align:center; }
.card { background:#1e1e1e; margin:15px; padding:15px; border-radius:12px; }
button { padding:10px 20px; margin:5px; border-radius:10px; border:none; }
.on { background:#2ecc71; }
.off { background:#e74c3c; }
</style>
</head>
<body>
<div class="card">
<h1>⏰ Alarm Clock</h1>
)rawliteral";

  html += "<h2>Time: " + String(h) + ":" +
          (m < 10 ? "0" : "") + String(m) + "</h2>";

  html += "<p>Motor: " + String(manualMotorState ? "ON 🟢" : "OFF 🔴") + "</p>";

  html += R"rawliteral(
<a href='/on'><button class='on'>ON</button></a>
<a href='/off'><button class='off'>OFF</button></a>
</div>

<div class="card">
<h2>Alarms</h2>
)rawliteral";

  for (int i = 0; i < MAX_ALARMS; i++) {
    if (alarms[i].enabled) {
      html += "<p>";
      html += "Alarm " + String(i) + ": ";
      html += String(alarms[i].hour) + ":" +
              (alarms[i].minute < 10 ? "0" : "") +
              String(alarms[i].minute);

      html += " <a href='/del?id=" + String(i) + "'>[delete]</a>";
      html += "</p>";
    }
  }

  html += R"rawliteral(
<h3>Set Alarm</h3>
<form action="/set" method="POST">
)rawliteral";

  html += "ID: <input name='id' type='number' min='0' max='4'><br>";
  html += "Hour: <input name='hour' type='number' min='0' max='23'><br>";
  html += "Minute: <input name='minute' type='number' min='0' max='59'><br><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form></div></body></html>";

  return html;
}

// ---------------- HANDLERS ----------------
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", page());
}

void handleOn() {
  motorOn();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  motorOff();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSet() {
  int id = server.arg("id").toInt();

  if (id >= 0 && id < MAX_ALARMS) {
    alarms[id].hour = server.arg("hour").toInt();
    alarms[id].minute = server.arg("minute").toInt();
    alarms[id].enabled = true;
    alarms[id].active = false;
  }

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDelete() {
  int id = server.arg("id").toInt();

  if (id >= 0 && id < MAX_ALARMS) {
    alarms[id].enabled = false;
    alarms[id].active = false;
  }

  server.sendHeader("Location", "/");
  server.send(303);
}

// ---------------- ALARM ENGINE ----------------
void checkAlarms() {
  int h = (timeClient.getHours() + 2) % 24;
  int m = timeClient.getMinutes();
  unsigned long now = millis();

  for (int i = 0; i < MAX_ALARMS; i++) {

    // START alarm
    if (alarms[i].enabled &&
        !alarms[i].active &&
        alarms[i].hour == h &&
        alarms[i].minute == m) {

      alarms[i].active = true;
      alarms[i].startMillis = now;
      motorOn();
    }

    // STOP after 5 min
    if (alarms[i].active &&
        (now - alarms[i].startMillis >= RING_TIME)) {

      alarms[i].active = false;
      motorOff();
    }
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  motorOff();

  for (int i = 0; i < MAX_ALARMS; i++) {
    alarms[i] = {7, 30, false, false, 0};
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(300);

  timeClient.begin();

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/set", handleSet);
  server.on("/del", handleDelete);

  server.begin();
}

// ---------------- LOOP ----------------
void loop() {
  server.handleClient();
  timeClient.update();

  if (millis() - lastCheck > 1000) {
    lastCheck = millis();
    checkAlarms();
  }
}
