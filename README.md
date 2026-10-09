# HuskyLens 2 + ESP32 Computer Vision Projects

Embedded computer-vision projects built on the **DFRobot HuskyLens 2** AI camera
and an **ESP32 Dev Module**. The camera runs on-device (edge) inference — face,
hand, licence-plate recognition and self-learning classification — and the ESP32
reads the results over I2C to drive real hardware (servos, locks, a browser game).

| Project | HuskyLens 2 algorithm | What it does |
|---|---|---|
| [`face-recognition-lock`](face-recognition-lock) | Face Recognition | Unlocks (GPIO 13 HIGH) when the enrolled face (ID 1) is seen; auto-relocks 3 s after the face leaves. |
| [`parking_barrier`](parking_barrier) | Licence Plate Recognition | Raises a servo barrier for an authorised number plate, closes it for any other plate. |
| [`recycle-sorter`](recycle-sorter) | Self-Learning Classification | Classifies waste held up to the camera as **recycle** or **general** by its learned name, debounces over 5 frames and swings a sorting servo. |
| [`flappy-bird-hand-control`](flappy-bird-hand-control) | Hand Recognition | Play Flappy Bird by raising your hand — the ESP32 tracks hand height and streams flap events over USB to a browser game via the Web Serial API. |

## Hardware

- DFRobot HuskyLens 2 (set *System Settings → Protocol Type → I2C*)
- ESP32 Dev Module
- SG90-style servo / LED depending on the project

Wiring (all projects):

| HuskyLens 2 | ESP32 |
|---|---|
| green (SDA) | GPIO 21 |
| blue (SCL)  | GPIO 22 |
| red (+)     | 5V / VIN |
| black (−)   | GND |

Servo / LED output on **GPIO 13**. Servos are driven directly with the ESP32 LEDC
peripheral (50 Hz, 14-bit duty) — no servo library needed.

## Software

- Arduino IDE 2 with the **esp32** board package (3.x)
- Library: [`DFRobot_HuskylensV2`](https://github.com/DFRobot/DFRobot_HuskylensV2) (not the old v1 `HUSKYLENS` library)

Open the `.ino` in each folder, select **ESP32 Dev Module**, upload, and open the
Serial Monitor at 115200 baud. Each sketch's header comment has the exact
HuskyLens training steps.

For the Flappy Bird game, close the Serial Monitor after uploading, open
`flappy_game.html` in Chrome or Edge and click **Connect ESP32**.

## Related

- [GESTURE-CONTROL-using-huskeylense](https://github.com/Dhanip200/GESTURE-CONTROL-using-huskeylense) — gesture control with HuskyLens
