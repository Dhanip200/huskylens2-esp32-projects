/*
  Recycle Sorter - HuskyLens 2 + ESP32 Dev Module

  Students hold a piece of waste in front of the HuskyLens 2.
  Serial Monitor prints:
    R  -> an item whose learned name is "recycle"
    G  -> an item whose learned name is "general"
  Anything else (e.g. the empty background) prints nothing.
  Servo on GPIO 13: "recycle" -> 90 degrees, anything else (general / obj /
  nothing in view) -> 0 degrees.
  A letter is printed once per item, after the result has been stable for
  STABLE_FRAMES readings, so it does not flicker between R and G.

  Wiring (I2C):
    HuskyLens 2 green (SDA) -> ESP32 GPIO 21
    HuskyLens 2 blue  (SCL) -> ESP32 GPIO 22
    HuskyLens 2 red   (+)   -> ESP32 5V/VIN (or 3V3)
    HuskyLens 2 black (-)   -> ESP32 GND
    Servo signal (orange/yellow) -> GPIO 13, red -> 5V, brown/black -> GND

  HuskyLens 2 setup:
    1. System Settings > Protocol Type > I2C
    2. Open "Self-Learning Classification"
    3. Learn each item as its own ID (bottle = ID 1, can = ID 2, ...)
    4. Name every recyclable ID "recycle" and every general-waste ID "general"
       (same name on many IDs is fine - the code checks the name, not the ID)
    5. Learn one more ID for the empty background and name it "nothing"

  Library: "DFRobot_HuskylensV2" (NOT the old "HUSKYLENS" v1 library)
    https://github.com/DFRobot/DFRobot_HuskylensV2
*/

#include <Wire.h>
#include "DFRobot_HuskylensV2.h"

const int SDA_PIN = 21;
const int SCL_PIN = 22;
const int SERVO_PIN = 13;
const int NONE = 0;
const int RECYCLE = 1;
const int GENERAL = 2;
const int STABLE_FRAMES = 5;  // same answer this many times in a row before printing

HuskylensV2 huskylens;
int lastPrinted = NONE;
int candidate = NONE;
int stableCount = 0;

// Move the servo to 0..180 degrees (50 Hz signal, 0.5 ms - 2.5 ms pulse)
void servoWrite(int angle) {
  int pulseUs = map(angle, 0, 180, 500, 2500);
  ledcWrite(SERVO_PIN, pulseUs * 16383L / 20000);  // 14-bit duty, 20 ms period
}

// Turn a learned name into RECYCLE / GENERAL / NONE (not case sensitive)
int categoryOf(String name) {
  name.trim();
  name.toLowerCase();
  if (name == "recycle") return RECYCLE;
  if (name == "general") return GENERAL;
  return NONE;
}

void setup() {
  Serial.begin(115200);
  ledcAttach(SERVO_PIN, 50, 14);
  servoWrite(0);

  Wire.begin(SDA_PIN, SCL_PIN);
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens 2 not found: check wiring and set Protocol Type to I2C");
    delay(1000);
  }
  huskylens.switchAlgorithm(ALGORITHM_SELF_LEARNING_CLASSIFICATION);
  delay(2000);  // give the HuskyLens time to switch algorithm
  Serial.println("HuskyLens 2 ready - show me some waste!");
}

void loop() {
  int seen = NONE;

  if (huskylens.getResult(ALGORITHM_SELF_LEARNING_CLASSIFICATION) > 0) {
    while (huskylens.available(ALGORITHM_SELF_LEARNING_CLASSIFICATION)) {
      Result *result = huskylens.popCachedResult(ALGORITHM_SELF_LEARNING_CLASSIFICATION);
      if (result && result->ID > 0) {  // ID 0 = not a learned item
        seen = categoryOf(result->name);
      }
    }
  }

  // Debounce: only accept a result once it has been the same for STABLE_FRAMES
  if (seen == candidate) {
    if (stableCount < STABLE_FRAMES) stableCount++;
  } else {
    candidate = seen;
    stableCount = 1;
  }

  if (stableCount == STABLE_FRAMES && candidate != lastPrinted) {
    lastPrinted = candidate;  // goes back to NONE when the item is removed
    servoWrite(candidate == RECYCLE ? 90 : 0);
    if (candidate == RECYCLE) Serial.println("R");
    else if (candidate == GENERAL) Serial.println("G");
  }

  delay(100);
}
