/*
  Face Recognition Lock - HuskyLens 2 + ESP32 Dev Module

  When the HuskyLens 2 recognises learned face ID 1, GPIO 13 goes HIGH
  (LED on / unlock). It stays HIGH while the face is visible and for
  UNLOCK_HOLD_MS after the face disappears, then goes LOW again.

  Wiring (I2C):
    HuskyLens 2 green (SDA) -> ESP32 GPIO 21
    HuskyLens 2 blue  (SCL) -> ESP32 GPIO 22
    HuskyLens 2 red   (+)   -> ESP32 5V/VIN (or 3V3)
    HuskyLens 2 black (-)   -> ESP32 GND
    LED (+ via 220R)        -> ESP32 GPIO 13, LED (-) -> GND

  HuskyLens 2 setup:
    1. System Settings > Protocol Type > I2C
    2. Open "Face Recognition" and learn your face -> it becomes ID 1

  Library: "DFRobot_HuskylensV2" (NOT the old "HUSKYLENS" v1 library)
    https://github.com/DFRobot/DFRobot_HuskylensV2
*/

#include <Wire.h>
#include "DFRobot_HuskylensV2.h"

const int LOCK_PIN = 13;
const int SDA_PIN = 21;
const int SCL_PIN = 22;
const int AUTHORIZED_ID = 1;
const unsigned long UNLOCK_HOLD_MS = 3000;  // stay on this long after face leaves

HuskylensV2 huskylens;
unsigned long lastSeenMs = 0;
bool unlocked = false;

void setLock(bool open) {
  if (open == unlocked) return;
  unlocked = open;
  digitalWrite(LOCK_PIN, open ? HIGH : LOW);
  Serial.println(open ? "Face ID 1 recognised -> pin 13 HIGH"
                      : "pin 13 LOW");
}

void setup() {
  Serial.begin(115200);
  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens 2 not found: check wiring and set Protocol Type to I2C");
    delay(1000);
  }
  huskylens.switchAlgorithm(ALGORITHM_FACE_RECOGNITION);
  delay(2000);  // give the HuskyLens time to switch algorithm
  Serial.println("HuskyLens 2 ready - face recognition mode");
}

void loop() {
  bool authorizedSeen = false;

  // getResult() fetches the latest results; returns > 0 when faces are seen
  if (huskylens.getResult(ALGORITHM_FACE_RECOGNITION) > 0) {
    while (huskylens.available(ALGORITHM_FACE_RECOGNITION)) {
      Result *result = huskylens.popCachedResult(ALGORITHM_FACE_RECOGNITION);
      if (result) {
        Serial.print("Face ID: ");
        Serial.println(result->ID);
        if (result->ID == AUTHORIZED_ID) authorizedSeen = true;
      }
    }
  }

  if (authorizedSeen) {
    lastSeenMs = millis();
    setLock(true);
  } else if (unlocked && millis() - lastSeenMs > UNLOCK_HOLD_MS) {
    setLock(false);
  }

  delay(100);
}
