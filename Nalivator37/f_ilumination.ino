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
        strip.setLED(curPumping, mWHEEL(col));           // зажгли цвет
        strip.show();
        col += 450;
        if (col > 1350) col = 0;

      }
    } else {
      strip.setLED(led , mWHEEL(col));
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
      if (shotStates[led] == READY) strip.setLED(led, mCOLOR(LIME));                    // налитая рюмка, статус: готов
      else if (shotStates[led] == EMPTY) strip.setLED(led, mCOLOR(RED));
      else if (shotStates[led] == NO_GLASS) strip.setLED(led, mCOLOR(BLACK));
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
