# LightweightServo

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://github.com/ArminJo/LightweightServo/workflows/LibraryBuild/badge.svg)](https://github.com/ArminJo/LightweightServo/actions)
[![Hit Counter](https://hitcounter.pythonanywhere.com/count/tag.svg?url=https%3A%2F%2Fgithub.com%2FArminJo%2FLightweightServo)](https://github.com/brentvollebregt/hit-counter)

## Small servo library for Arduino
 *  comparable to [Adafruit_TiCoServo library](https://github.com/adafruit/Adafruit_TiCoServo)
 *  Enables use of NeoPixels and servos in the same Arduino sketch
 *  Lightweight Servo implementation only for pin 9 and 10.
 *  Only timer1 hardware and no interrupts or other overhead used.
 *  Provides auto initialization.
 *  300 bytes code size / 4 bytes RAM including auto initialization compared to 700 / 48 bytes for Arduino Servo library.
 *  8 bytes for each call to setLightweightServoPulse...

## Usage
As simple as this:
```
void setup() {
    // no initialization needed for LightweightServo :-)
}

void loop() {
    setLightweightServoPulsePin9(0);
    delay(1000);
    setLightweightServoPulsePin9(180);
    delay(1000);
}
```
