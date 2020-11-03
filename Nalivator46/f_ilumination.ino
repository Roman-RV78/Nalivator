void CvetoMuzik() {
  static uint8_t led = 0;
  static uint8_t count = 0;
  static int16_t col = 0;
#ifdef LED_TOWER
  static bool flag = false;
  static uint8_t count2 = 0;
  static uint8_t count3 = 0;
  static uint8_t count4 = 0;
#endif
  if ( (MenuFlag == 15 || ledShow || systemState == PUMPING ) &&  LEDtimer.isReady()) {
    if (systemState == PUMPING) {
      if (++count > 3) {
        count = 0;
#ifndef LED_CHANGE_DIRECTION
        strip.setLED(curPumping, mWHEEL(col));           // зажгли цвет
#else
        strip.setLED(NUM_SHOTS - 1 - curPumping, mWHEEL(col));           // зажгли цвет
#endif
        strip.show();
        col += 450;
        if (col > 1350) col = 0;

      }
    } else {
#ifndef LED_CHANGE_DIRECTION
      strip.setLED(led , mWHEEL(col));
#else
      strip.setLED(NUM_SHOTS - 1 - led , mWHEEL(col));
#endif
      strip.show();
      if (++led >= NUM_SHOTS) {
        led = 0;
        col += 450;
        if (col > 1350) {
          col = 0;
          if (++count == 6) {  // колличество проходов иллюминации после налива
            count = 0;
            if (MenuFlag != 15 && !playMush) check = true; // если не в меню настроек, то проверяем рюмки

          }
        }
      }
    }
  }


  if (check) { // проверка рюмок
    for (led = 0; led < NUM_SHOTS; led++) {
#ifndef LED_CHANGE_DIRECTION
      if (shotStates[led] == READY) strip.setLED(led, mRGB(0, 255, 0));                    // налитая рюмка, статус: готов
      else if (shotStates[led] == EMPTY) strip.setLED(led, mRGB(255, 0, 0));
      else if (shotStates[led] == NO_GLASS) strip.setLED(led, mRGB(0, 0, 0));
#else
      if (shotStates[led] == READY) strip.setLED(NUM_SHOTS - 1 - led, mRGB(0, 255, 0));                    // налитая рюмка, статус: готов
      else if (shotStates[led] == EMPTY) strip.setLED(NUM_SHOTS - 1 - led, mRGB(255, 0, 0));
      else if (shotStates[led] == NO_GLASS) strip.setLED(NUM_SHOTS - 1 - led, mRGB(0, 0, 0));
#endif
    }
    LEDtimer.reset();
    LEDchanged = true;
    check = false;
    ledShow = false;
    led = 0;
    col = 0;
    count = 0;
  }
  // отрисовка светодиодов по флагу
  if (!ledShow && LEDchanged && LEDtimer.isReady()) {
    LEDchanged = false;
    strip.show();
  }
  // Башня LED
#ifdef LED_TOWER
  if (mig && !clearLed && TOWERtimer.isReady()) {

    if (flag ) {
      leds2[count2] = mRGB(255, 0, 0);
      leds2[count2 + (NUMLEDS / 2)] = mRGB(0, 0, 255);
    } else {
      leds2[count2 + NUMLEDS / 2] = mRGB(255, 0, 0);
      leds2[count2] = mRGB(0, 0, 255);
    }
    strip2.show();
    if (++count2 >= NUMLEDS / 2 ) {
      count2 = 0;
      flag = !flag;
    }

    if (++count3 > 90 ) { // продолжительность мигалки у.е.
      mig = false;
      clearLed = true;
      TOWERtimer.reset();
    }
  }


  if ((MenuFlag == 15 || rainbow ) && !clearLed && TOWERtimer.isReady()) {
    leds2[count2] = mHSV((uint8_t)(count3 + count2 * (255 / NUMLEDS)), 255, 255); //
    count3 += 4;
    strip2.show();
    if (++count2 >= NUMLEDS ) {
      count2 = 0;
      if (!playMush && MenuFlag != 15 && ++count4 > 25 ) { // продолжительность радуги у.е.
        rainbow = false;
        clearLed = true;
        TOWERtimer.reset();
      }
    }

  }

  if (systemON && ledShowOn && !clearLed && !promivka && MenuFlag != 20 && TOWERtimer.isReady()) {
    if (++count2 >= 7) count2 = 0;
    if (++count4 >= 21) count4 = 0;
    strip2.clear();
    for (count3 = count2; count3 < NUMLEDS; count3 += 7) {
      if (count4 < 7 ) {
        strip2.setRGB(count3, 255, 0, 0);
      } else if (count4 < 14 ) {
        strip2.setRGB(count3, 0, 255, 0);
      } else if (count4 < 21 ) {
        strip2.setRGB(count3, 0, 0, 255);
      }
    }
    strip2.show();

  }

  if (clearLed && TOWERtimer.isReady()) {
    clearLed = false;
    count2 = 0;
    count3 = 0;
    count4 = 0;
    strip2.clear();
    strip2.show();
    TOWERtimer.reset();
  }


#endif
}
