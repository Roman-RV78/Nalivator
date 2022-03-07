void CvetoMuzik() {
  static uint8_t led = 0;
  static uint8_t count = 0;
  static uint8_t counter = 0;
#ifdef LED_TOWER
  static bool flag = false;
  static uint8_t count2 = 0;
  static uint8_t count3 = 0;
  static uint8_t count4 = 0;
#endif
  if ( (MenuFlag == 15 || ledShow || systemState == PUMPING ) &&  LEDtimer.isReady()) {
    if (systemState == PUMPING) {
#ifndef LED_CHANGE_DIRECTION
      strip.setLED(curPumping, mHSV(counter, 255, 255));           // радуга при наливе рюмки
#else
      strip.setLED(NUM_SHOTS - 1 - curPumping, mHSV(counter, 255, 255));           // радуга при наливе рюмки
#endif
      counter += INTENSITY_COLOR_CHANGE_GLASS; // можно поменять , интенсивность смены цвета при розливе на рюмке
      strip.show();
    } else {
#ifndef LED_CHANGE_DIRECTION
      leds[led] = mHSV((uint8_t)(counter + led * TABLE_RAINBOW_STEP) , 255, 255); //  умножение  led  увеличивает шаг радуги стола, можно поиграться значением
#else
      leds[NUM_SHOTS - 1 - led] = mHSV((uint8_t)(counter + (NUM_SHOTS - 1 - led) * TABLE_RAINBOW_STEP) , 255, 255); //  умножение  led  увеличивает шаг радуги стола, можно поиграться значением
#endif
      counter += 5;
      strip.show();

      if (++led >= NUM_SHOTS) {
        led = 0;
        if (++count == NUMBER_ILLUMINATION_PASSES) {  // количество проходов иллюминации после налива
          count = 0;
          if (MenuFlag != 15 && !playMush) check = true; // если не в меню настроек, то проверяем рюмки

        }
      }
    }
  }


  if (check) { // проверка рюмок
    for (led = 0; led < NUM_SHOTS; led++) {
#ifndef LED_CHANGE_DIRECTION
  #ifdef TWO_PUMPS
      if (shotStates[led] == EMPTY) {
        if (pourFrom[led] == 0) strip.setLED(led, mRGB(COLOR_EMPTY_GLASS_1)); // подсветили
        else if (pourFrom[led] == 1) strip.setLED(led, mRGB(COLOR_EMPTY_GLASS_2)); // подсветили
      }
  #else
      if (shotStates[led] == EMPTY) strip.setLED(led, mRGB(COLOR_EMPTY_GLASS_1));                    // налитая рюмка, статус: готов
  #endif
      else if (shotStates[led] == READY) strip.setLED(led, mRGB(COLOR_POURET_GLASS));
      else if (shotStates[led] == NO_GLASS) strip.setLED(led, mRGB(0, 0, 0));
#else
  #ifdef TWO_PUMPS
      if (shotStates[led] == EMPTY) {
        if (pourFrom[led] == 0) strip.setLED(NUM_SHOTS - 1 - led, mRGB(COLOR_EMPTY_GLASS_1));                    // налитая рюмка, статус: готов
        else if (pourFrom[led] == 1) strip.setLED(NUM_SHOTS - 1 - led, mRGB(COLOR_EMPTY_GLASS_2));                    // налитая рюмка, статус: готов
      }
  #else
      if (shotStates[led] == EMPTY) strip.setLED(NUM_SHOTS - 1 - led, mRGB(COLOR_EMPTY_GLASS_1));                    // налитая рюмка, статус: готов
  #endif
      else if (shotStates[led] == READY) strip.setLED(NUM_SHOTS - 1 - led, mRGB(COLOR_POURET_GLASS));
      else if (shotStates[led] == NO_GLASS) strip.setLED(NUM_SHOTS - 1 - led, mRGB(0, 0, 0));
#endif
    }
    LEDtimer.setInterval(GLASS_RAINBOW_ILLUMINATION_TIMER);
    LEDtimer.reset();
    LEDchanged = true;
    check = false;
    ledShow = false;
    led = 0;
    count = 0;
    counter = 0;
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

    if (++count3 > 75 ) { // продолжительность мигалки у.е.
      mig = false;
      clearLed = true;
      TOWERtimer.reset();
    }
  }


  if ((MenuFlag == 15 || rainbow ) && !clearLed && TOWERtimer.isReady()) {
    leds2[count2] = mHSV((uint8_t)(count3 + count2 * 50 ), 255, 255); //  умножение  count2  увеличивает шаг радуги башни, можно поиграться значением
    count3 += 5;
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

  if (systemON && (ledShowOn == 1) && !clearLed && !promivka && MenuFlag != 20 && TOWERtimer.isReady()) {
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
