/*
  Flappy Bird with HuskyLens 2 + ESP32
  Move your HAND up -> the bird flaps.

  Wiring: HuskyLens green -> GPIO 21, blue -> GPIO 22, red -> 5V, black -> GND
  HuskyLens: System Settings > Protocol Type > I2C
  Then close the Serial Monitor, open flappy_game.html in Chrome/Edge
  and click "Connect ESP32".
*/

#include <Wire.h>
#include "DFRobot_HuskylensV2.h"

HuskylensV2 huskylens;

int restY = -1;          // where your hand normally is (-1 = not known yet)
bool flapped = false;    // so we flap only once per hand movement

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  while (!huskylens.begin(Wire)) {
    Serial.println("HuskyLens not found");
    delay(1000);
  }
  huskylens.switchAlgorithm(ALGORITHM_HAND_RECOGNITION);
  delay(2000);
}

void loop() {
  if (huskylens.getResult(ALGORITHM_HAND_RECOGNITION) > 0) {
    Result *hand = huskylens.getCachedCenterResult(ALGORITHM_HAND_RECOGNITION);
    if (hand) {
      int y = hand->yCenter;           // smaller y = hand higher
      if (restY < 0) restY = y;

      if (restY - y > 30 && !flapped) {  // hand went up -> flap
        Serial.println("F");
        flapped = true;
      }
      if (restY - y < 10) {              // hand back down -> ready again
        flapped = false;
      }

      restY = (restY * 9 + y) / 10;      // slowly follow your normal hand position
    }
  } else {
    restY = -1;                          // no hand: start again when it comes back
  }
  delay(20);
}
