/*
  Automatic Parking Barrier - HuskyLens 2 + ESP32
  The learned number plate (ID 1) is seen -> servo on pin 13 goes to 90 (open)
  Any other plate / no plate              -> servo goes to 0 (closed)

  Wiring: HuskyLens green -> GPIO 21, blue -> GPIO 22, red -> 5V, black -> GND
          Servo signal (orange/yellow) -> GPIO 13, red -> 5V, brown/black -> GND

  HuskyLens 2 setup:
    1. System Settings > Protocol Type > I2C
    2. Open "License Plate Recognition", point it at the allowed plate
       and learn it -> it becomes ID 1
*/

#include <Wire.h>
#include "DFRobot_HuskylensV2.h"

const int SERVO_PIN = 13;
const int ALLOWED_ID = 1;
const unsigned long OPEN_HOLD_MS = 3000;  // stay open this long after the plate leaves

HuskylensV2 huskylens;
unsigned long lastSeenMs = 0;

// Move the servo to 0..180 degrees (50 Hz signal, 0.5 ms - 2.5 ms pulse)
void servoWrite(int angle) {
  int pulseUs = map(angle, 0, 180, 500, 2500);
  ledcWrite(SERVO_PIN, pulseUs * 16383L / 20000);  // 14-bit duty, 20 ms period
}

void setup() {
  Serial.begin(115200);
  ledcAttach(SERVO_PIN, 50, 14);
  servoWrite(0);

  Wire.begin(21, 22);
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens not found");
    delay(1000);
  }
  huskylens.switchAlgorithm(ALGORITHM_LICENSE_RECOGNITION);
  delay(2000);
}

void loop() {
  bool allowedSeen = false;

  if (huskylens.getResult(ALGORITHM_LICENSE_RECOGNITION) > 0) {
    while (huskylens.available(ALGORITHM_LICENSE_RECOGNITION)) {
      Result *plate = huskylens.popCachedResult(ALGORITHM_LICENSE_RECOGNITION);
      if (plate) {
        Serial.print("Plate ID: ");
        Serial.print(plate->ID);
        Serial.print("  text: ");
        Serial.println(plate->content);
        if (plate->ID == ALLOWED_ID) allowedSeen = true;
      }
    }
  }

  if (allowedSeen) lastSeenMs = millis();

  if (lastSeenMs != 0 && millis() - lastSeenMs < OPEN_HOLD_MS) {
    servoWrite(90);   // open
  } else {
    servoWrite(0);    // closed
  }

  delay(100);
}
