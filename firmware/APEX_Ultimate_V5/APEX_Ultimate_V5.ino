/* APEX Ultimate V5. Preserve the baseline hardware interfaces; see review doc.
 * Only controlTask owns decisions, motor outputs and servo PWM after setup.
 * Motor output gate defaults to BENCH; Config.h documents physical enable.
 */
#include <Arduino.h>
#include <U8g2lib.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>
#include <esp_arduino_version.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "Config.h"
#include "ControlCore.h"
#include "Dashboard.h"

#if ESP_ARDUINO_VERSION_MAJOR < 3 || !defined(CONFIG_IDF_TARGET_ESP32)
#error "Use Arduino-ESP32 3.x with a classic ESP32/WROOM board."
#endif

WebServer server(80);
U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE);
QueueHandle_t commands, acknowledgements;
SemaphoreHandle_t snapshotMutex;
TaskHandle_t controlHandle;
struct Snapshot {
  ApexCore core;
  int outputLeft, outputRight;
  bool pwmOK;
} snapshot;
bool oledOK = false;
uint32_t nextTicket = 1;

// Echo capture owns only these pulse fields, never controller state or motors.
portMUX_TYPE echoMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool echoWaiting = false, echoRose = false, echoDone = false;
volatile uint32_t echoRiseUs = 0, echoWidthUs = 0;
void ARDUINO_ISR_ATTR echoISR() {
  uint32_t t = micros();
  bool high = digitalRead(ECHO);
  portENTER_CRITICAL_ISR(&echoMux);
  if (echoWaiting) {
    if (high && !echoRose) {
      echoRose = true;
      echoRiseUs = t;
    } else if (!high && echoRose) {
      echoWidthUs = t - echoRiseUs;
      echoDone = true;
      echoWaiting = false;
    }
  }
  portEXIT_CRITICAL_ISR(&echoMux);
}
void updateUltrasonic(ApexInput& f, int pan, uint32_t now) {
  static bool active = false;
  static uint32_t startedUs = 0, startedAt = 0, lastTrigger = 0;
  static int samplePan = 90;
  bool done;
  uint32_t width;
  portENTER_CRITICAL(&echoMux);
  done = echoDone;
  width = echoWidthUs;
  if (done) echoDone = false;
  portEXIT_CRITICAL(&echoMux);
  bool timeout = active && uint32_t(micros() - startedUs) > 25000;
  if (active && (done || timeout)) {
    portENTER_CRITICAL(&echoMux);
    echoWaiting = false;
    echoRose = false;
    portEXIT_CRITICAL(&echoMux);
    f.at = startedAt;
    f.rangePan = samplePan;
    f.sampleId++;
    f.cm = done ? width * .0343f / 2.0f : 0;
    f.rangeValid = done && f.cm >= 2 && f.cm <= 400;
    active = false;
  }
  if (!active && uint32_t(now - lastTrigger) >= 70) {
    lastTrigger = now;
    startedAt = now;
    samplePan = pan;
    startedUs = micros();
    active = true;
    portENTER_CRITICAL(&echoMux);
    echoWaiting = true;
    echoRose = false;
    echoDone = false;
    echoWidthUs = 0;
    portEXIT_CRITICAL(&echoMux);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
  }
}
void updateIR(ApexInput& f, uint32_t now) {
  static int candidateL = -1, candidateR = -1;
  static uint32_t changedAt = 0;
  int l = digitalRead(IR_LEFT), r = digitalRead(IR_RIGHT);
  if (l != candidateL || r != candidateR) {
    candidateL = l;
    candidateR = r;
    changedAt = now;
  }
  f.irStable = uint32_t(now - changedAt) >= 40;
  if (f.irStable) {
    f.irLeft = l;
    f.irRight = r;
  }
}
int servoPulse(int angle) {
  int delta = angle - 90;
  if (!SERVO_REVERSED) delta = -delta;
  int us =
      delta >= 0
          ? SERVO_CENTER_US + delta * (SERVO_MAX_US - SERVO_CENTER_US) / 90
          : SERVO_CENTER_US + delta * (SERVO_CENTER_US - SERVO_MIN_US) / 90;
  return constrain(us, SERVO_MIN_US, SERVO_MAX_US);
}
void disableMotorPins() {
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void oneMotor(int a, int b, int pwmPin, int logical, bool reverse) {
  int electrical = reverse ? -logical : logical;
  digitalWrite(a, electrical > 0);
  digitalWrite(b, electrical < 0);
  ledcWrite(pwmPin, abs(logical));
}
void applyMotors(int l, int r, uint32_t now, int& outL, int& outR) {
  static int previousSignL = 0, previousSignR = 0;
  static uint32_t neutralAt = 0;
  static bool neutral = false;
  outL = outR = 0;
  if (!MOTOR_POWER_VERIFIED) {
    disableMotorPins();
    return;
  }
  int sl = (l > 0) - (l < 0), sr = (r > 0) - (r < 0);
  if (sl != previousSignL || sr != previousSignR) {
    disableMotorPins();
    previousSignL = sl;
    previousSignR = sr;
    neutralAt = now;
    neutral = true;
  }
  if (!l && !r) {
    disableMotorPins();
    neutral = false;
    return;
  }
  if (neutral && uint32_t(now - neutralAt) < 40) {
    disableMotorPins();
    return;
  }
  neutral = false;
  oneMotor(IN1, IN2, ENA, l, LEFT_MOTOR_REVERSED);
  oneMotor(IN3, IN4, ENB, r, RIGHT_MOTOR_REVERSED);
  outL = l;
  outR = r;
}
void controlTask(void*) {
  ApexCore c;
  apex_defaults(&c);
  c.cfg.settleMs = SERVO_SETTLE_MS;
  c.cfg.safeTravel = SERVO_SAFE_TRAVEL_DEG;
  c.cfg.blackLeft = IR_BLACK_LEFT;
  c.cfg.blackRight = IR_BLACK_RIGHT;
  c.cfg.panMounted = SENSOR_ON_SERVO;
  ApexInput frame = {};
  int previousPan = -1, outL = 0, outR = 0;
  uint32_t previousTick = millis();
  TickType_t wake = xTaskGetTickCount();
  for (;;) {
    uint32_t now = millis();
    if (uint32_t(now - previousTick) > 100)
      apex_stop(&c, "Control-loop deadline missed: stopped");
    previousTick = now;
    uint32_t urgent = 0;
    if (xTaskNotifyWait(0, UINT32_MAX, &urgent, 0) == pdTRUE && urgent)
      apex_stop(&c, "Emergency command received: stopped");
    updateUltrasonic(frame, SENSOR_ON_SERVO ? c.pan : 90, now);
    updateIR(frame, now);
    c.in = frame;
    ApexCommand command;
    for (int i = 0; i < 4 && xQueueReceive(commands, &command, 0) == pdTRUE;
         i++) {
      ApexAck ack = apex_command(&c, &command, now);
      xQueueOverwrite(acknowledgements, &ack);
    }
    apex_tick(&c, &frame, now);
    if (c.pan != previousPan) {
      ledcWrite(SERVO_PIN,
                (uint32_t(servoPulse(c.pan)) * 65535UL + 10000UL) / 20000UL);
      previousPan = c.pan;
    }
    applyMotors(c.left, c.right, now, outL, outR);
    if (xSemaphoreTake(snapshotMutex, pdMS_TO_TICKS(2)) == pdTRUE) {
      snapshot.core = c;
      snapshot.outputLeft = outL;
      snapshot.outputRight = outR;
      snapshot.pwmOK = true;
      xSemaphoreGive(snapshotMutex);
    }
    vTaskDelayUntil(&wake, pdMS_TO_TICKS(5));
  }
}
Snapshot readSnapshot() {
  Snapshot s;
  xSemaphoreTake(snapshotMutex, portMAX_DELAY);
  s = snapshot;
  xSemaphoreGive(snapshotMutex);
  return s;
}
void escapeJSON(const char* in, char* out, size_t size) {
  size_t j = 0;
  for (size_t i = 0; in[i] && j + 2 < size; i++) {
    unsigned char c = in[i];
    if (c < 32) continue;
    if (c == '"' || c == '\\') out[j++] = '\\';
    out[j++] = c;
  }
  out[j] = 0;
}
void sendError(int code, const char* message) {
  char escaped[220], body[300];
  escapeJSON(message, escaped, sizeof(escaped));
  snprintf(body, sizeof(body), "{\"ok\":false,\"message\":\"%s\"}", escaped);
  server.sendHeader("Cache-Control", "no-store");
  server.send(code, "application/json", body);
}
bool parseUnsigned(const String& v, uint32_t& n) {
  if (!v.length() || v.length() > 10) return false;
  for (size_t i = 0; i < v.length(); i++)
    if (v[i] < '0' || v[i] > '9') return false;
  unsigned long long x = strtoull(v.c_str(), nullptr, 10);
  if (x > UINT32_MAX) return false;
  n = x;
  return true;
}
bool parseSession(const String& v, uint64_t& n) {
  if (v.length() != 16) return false;
  for (size_t i = 0; i < v.length(); i++)
    if (!isxdigit((unsigned char)v[i])) return false;
  n = strtoull(v.c_str(), nullptr, 16);
  return n != 0;
}
bool integerArg(const char* key, int& n, int lo, int hi) {
  String v = server.arg(key);
  if (!v.length() || v.length() > 7) return false;
  size_t start = v[0] == '-' ? 1 : 0;
  if (start == v.length()) return false;
  for (size_t i = start; i < v.length(); i++)
    if (!isdigit((unsigned char)v[i])) return false;
  long x = strtol(v.c_str(), nullptr, 10);
  if (x < lo || x > hi) return false;
  n = x;
  return true;
}
bool dispatch(ApexCommand& q, ApexAck& ack) {
  q.ticket = nextTicket++;
  ApexAck old;
  while (xQueueReceive(acknowledgements, &old, 0) == pdTRUE) {
  }
  if (xQueueSend(commands, &q, 0) != pdTRUE) {
    xTaskNotify(controlHandle, 1, eSetBits);
    return false;
  }
  uint32_t started = millis();
  while (uint32_t(millis() - started) < 200) {
    if (xQueueReceive(acknowledgements, &ack, pdMS_TO_TICKS(20)) == pdTRUE &&
        ack.ticket == q.ticket)
      return true;
  }
  xTaskNotify(controlHandle, 1, eSetBits);
  return false;
}
void acknowledge(const ApexAck& ack) {
  char escaped[200], body[300];
  escapeJSON(ack.message, escaped, sizeof(escaped));
  snprintf(body, sizeof(body), "{\"ok\":%s,\"epoch\":%lu,\"message\":\"%s\"}",
           ack.ok ? "true" : "false", (unsigned long)ack.epoch, escaped);
  server.sendHeader("Cache-Control", "no-store");
  server.send(ack.ok ? 200 : 409, "application/json", body);
}
void sendStatus() {
  uint64_t token = 0;
  Snapshot s = readSnapshot();
  if (!parseSession(server.arg("token"), token) || token != s.core.session) {
    sendError(403, "Reconnect controller");
    return;
  }
  const ApexCore& c = s.core;
  char status[200], text[90], block[160], body[1900];
  escapeJSON(c.status, status, sizeof(status));
  escapeJSON(c.text, text, sizeof(text));
  escapeJSON(apex_forward_block(&c, millis()), block, sizeof(block));
  bool fresh = c.in.rangeValid && uint32_t(millis() - c.in.at) <= c.cfg.staleMs;
  snprintf(
      body, sizeof(body),
      "{\"version\":\"%s\",\"epoch\":%lu,\"mode\":%d,\"phase\":\"%s\","
      "\"armed\":%s,\"running\":%s,"
      "\"bench\":%s,\"status\":\"%s\",\"forward\":\"%s\",\"valid\":%s,\"cm\":%."
      "1f,\"angle\":%d,\"pulse\":%d,"
      "\"power\":%d,\"left\":%d,\"right\":%d,\"outputLeft\":%d,\"outputRight\":"
      "%d,"
      "\"irL\":%d,\"irR\":%d,\"irStable\":%s,\"blackL\":%d,\"blackR\":%d,"
      "\"calibrated\":%s,"
      "\"whiteSaved\":%s,\"blackSaved\":%s,\"text\":\"%s\",\"oled\":%s,"
      "\"minAngle\":%d,\"maxAngle\":%d,"
      "\"correction\":%d,\"stopCm\":%d,\"clearCm\":%d,\"whiteCenter\":%d,"
      "\"recoveryMs\":%lu,\"turnMs\":%lu}",
      FIRMWARE_VERSION, (unsigned long)c.epoch, int(c.mode),
      apex_phase_name(c.phase), c.armed ? "true" : "false",
      c.running ? "true" : "false", MOTOR_POWER_VERIFIED ? "false" : "true",
      status, block, fresh ? "true" : "false", c.in.cm, c.pan,
      servoPulse(c.pan), c.power, c.left, c.right, s.outputLeft, s.outputRight,
      c.in.irLeft, c.in.irRight, c.in.irStable ? "true" : "false",
      c.cfg.blackLeft, c.cfg.blackRight, c.calibrated ? "true" : "false",
      c.whiteSaved ? "true" : "false", c.blackSaved ? "true" : "false", text,
      oledOK ? "true" : "false", 90 - c.cfg.safeTravel, 90 + c.cfg.safeTravel,
      c.cfg.correction, c.cfg.stopCm, c.cfg.clearCm, c.cfg.whiteCenter,
      (unsigned long)c.cfg.recoverMs, (unsigned long)c.cfg.turnMs);
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", body);
}
void setupRoutes() {
  server.on("/", HTTP_GET, [] {
    server.sendHeader("Cache-Control", "no-store");
    server.send_P(200, "text/html", DASHBOARD);
  });
  server.on("/status", HTTP_GET, sendStatus);
  server.on("/claim", HTTP_POST, [] {
    ApexCommand q = {};
    q.type = CLAIM;
    do {
      q.session = (uint64_t(esp_random()) << 32) | esp_random();
    } while (!q.session);
    ApexAck ack;
    if (!dispatch(q, ack)) {
      sendError(503, "Control task did not acknowledge; stopped");
      return;
    }
    char body[160];
    snprintf(body, sizeof(body),
             "{\"ok\":true,\"token\":\"%016llx\",\"epoch\":%lu}",
             (unsigned long long)q.session, (unsigned long)ack.epoch);
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", body);
  });
  server.on("/cmd", HTTP_POST, [] {
    ApexCommand q = {};
    if (!parseSession(server.arg("token"), q.session) ||
        q.session != readSnapshot().core.session) {
      sendError(403, "Reconnect controller");
      return;
    }
    if (!parseUnsigned(server.arg("seq"), q.seq) || !q.seq ||
        !parseUnsigned(server.arg("epoch"), q.epoch)) {
      sendError(400, "Invalid command sequence/generation");
      return;
    }
    String kind = server.arg("kind");
    bool valid = true;
    if (kind == "arm")
      q.type = ARM;
    else if (kind == "start")
      q.type = START;
    else if (kind == "stop")
      q.type = STOP;
    else if (kind == "pause")
      q.type = PAUSE;
    else if (kind == "disarm")
      q.type = DISARM;
    else if (kind == "release")
      q.type = RELEASE;
    else if (kind == "lease")
      q.type = LEASE;
    else if (kind == "mode") {
      q.type = SET_MODE;
      valid = integerArg("value", q.a, 0, 2);
    } else if (kind == "power") {
      q.type = SET_POWER;
      valid = integerArg("value", q.a, 0, 255);
    } else if (kind == "pan") {
      q.type = SET_PAN;
      valid = integerArg("value", q.a, 0, 180);
    } else if (kind == "drive") {
      q.type = DRIVE;
      String v = server.arg("value");
      valid = v.length() == 1 && String("FBLR").indexOf(v[0]) >= 0;
      if (valid) q.a = v[0];
    } else if (kind == "text") {
      q.type = SET_TEXT;
      String v = server.arg("value");
      int j = 0;
      for (size_t i = 0; i < v.length() && j < 40; i++)
        if (v[i] >= 32 && v[i] <= 126) q.text[j++] = v[i];
      q.text[j] = 0;
    } else if (kind == "white")
      q.type = CAL_WHITE;
    else if (kind == "black")
      q.type = CAL_BLACK;
    else if (kind == "tune") {
      q.type = TUNE;
      valid = integerArg("correction", q.a, 0, 180) &&
              integerArg("stopCm", q.b, 15, 100) &&
              integerArg("whiteCenter", q.c, 0, 1) &&
              integerArg("recoveryMs", q.d, 300, 1500) &&
              integerArg("turnMs", q.e, 100, 600);
    } else
      valid = false;
    if (!valid) {
      sendError(400, "Invalid command or parameter");
      return;
    }
    if (q.type == STOP || q.type == PAUSE || q.type == DISARM)
      xTaskNotify(controlHandle, 1, eSetBits);
    ApexAck ack;
    if (!dispatch(q, ack)) {
      sendError(503, "Control task timeout: stopped");
      return;
    }
    acknowledge(ack);
  });
  server.onNotFound([] { sendError(404, "Not found"); });
}
void renderOLED() {
  static uint32_t drawnAt = 0;
  uint32_t now = millis();
  if (!oledOK || uint32_t(now - drawnAt) < 250) return;
  drawnAt = now;
  Snapshot s = readSnapshot();
  const ApexCore& c = s.core;
  char line[48];
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x10_tf);
  snprintf(line, sizeof(line), "APEX V5 %s",
           !MOTOR_POWER_VERIFIED ? "BENCH"
           : c.armed             ? "ARMED"
                                 : "STOPPED");
  oled.drawStr(0, 9, line);
  oled.drawStr(0, 19, apex_mode_name(c.mode));
  if (c.in.rangeValid && uint32_t(now - c.in.at) <= c.cfg.staleMs)
    snprintf(line, sizeof(line), "Distance: %.1f cm", c.in.cm);
  else
    snprintf(line, sizeof(line), "Distance: NO ECHO");
  oled.drawStr(0, 29, line);
  snprintf(line, sizeof(line), "%.21s", apex_phase_name(c.phase));
  oled.drawStr(0, 39, line);
  snprintf(line, sizeof(line), "IR %d/%d %s A%d", c.in.irLeft, c.in.irRight,
           c.left < c.right   ? "LEFT"
           : c.left > c.right ? "RIGHT"
                              : "CENTER",
           c.pan);
  oled.drawStr(0, 49, line);
  uint32_t page = (now / 2000) % 3;
  if (page == 0)
    snprintf(line, sizeof(line), "%.21s", c.status);
  else if (page == 1)
    snprintf(line, sizeof(line), "%.21s", c.text);
  else
    snprintf(line, sizeof(line), "%.19s",
             strlen(c.text) > 21 ? c.text + 21 : c.text);
  oled.drawStr(0, 61, line);
  oled.sendBuffer();
}
void setup() {
  Serial.begin(115200);
  for (int p : {ENA, IN1, IN2, IN3, IN4, ENB, TRIG}) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }
  pinMode(ECHO, INPUT);
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);
  commands = xQueueCreate(8, sizeof(ApexCommand));
  acknowledgements = xQueueCreate(1, sizeof(ApexAck));
  snapshotMutex = xSemaphoreCreateMutex();
  if (!commands || !acknowledgements || !snapshotMutex) {
    Serial.println("FATAL: RTOS resources");
    while (true) delay(1000);
  }
  apex_defaults(&snapshot.core);
  snapshot.outputLeft = snapshot.outputRight = 0;
  snapshot.pwmOK = false;
  bool a = ledcAttachChannel(ENA, 20000, 8, 0),
       b = ledcAttachChannel(ENB, 20000, 8, 1),
       c = ledcAttachChannel(SERVO_PIN, 50, 16, 4);
  if (!a || !b || !c) {
    Serial.println("FATAL: PWM setup");
    while (true) delay(1000);
  }
  disableMotorPins();
  attachInterrupt(digitalPinToInterrupt(ECHO), echoISR, CHANGE);
  if (xTaskCreatePinnedToCore(controlTask, "apexControl", 6144, nullptr, 3,
                              &controlHandle, 1) != pdPASS) {
    Serial.println("FATAL: control task");
    while (true) delay(1000);
  }
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setTimeOut(25);
  for (uint8_t address : {uint8_t(0x3C), uint8_t(0x3D)}) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      oled.setI2CAddress(address << 1);
      oled.begin();
      Wire.setTimeOut(25);
      oledOK = true;
      break;
    }
  }
  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    Serial.println("FATAL: AP");
    while (true) delay(1000);
  }
  setupRoutes();
  server.begin();
  Serial.printf(
      "APEX %s | %s | OLED %s\n", FIRMWARE_VERSION,
      MOTOR_POWER_VERIFIED ? "MOTOR OUTPUT ENABLED" : "BENCH: ENA/ENB DISABLED",
      oledOK ? "OK" : "MISSING");
  Serial.printf("WiFi: %s\nPassword: %s\nOpen http://", AP_SSID, AP_PASSWORD);
  Serial.println(WiFi.softAPIP());
}
void loop() {
  server.handleClient();
  renderOLED();
  delay(1);
}
