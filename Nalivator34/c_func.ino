

// наливайка, опрос концевиков
void flowTick() {
  if (FLOWdebounce.isReady()) {
    for (uint8_t i = 0; i < NUM_SHOTS; i++) {
      bool swState = !digitalRead(SW_pins[i]) ^ SWITCH_LEVEL;
      if (shotStates[i] == NO_GLASS && swState && !tost && !ledShow && !moving && !returnMenu) { // поставили пустую рюмку
        shotStates[i] = EMPTY;                                      // флаг на заправку
        strip.setLED(i, mCOLOR(RED));  // подсветили
        LEDchanged = true;
        //DEBUG("set glass");
        //DEBUG(i);
        if (!systemON && !save)play_track(i + 20); //  трек в папке mp3, с 20 по 25
        yesGlass++;
        if (save) {
          play_track(17);
          SAVEtimer.reset();
          save = false;
          enc.rst();
          lcd.backlight();
          check = true; //  проверяем рюмки
        }
        if (barMan && (MenuFlag == 1 || MenuFlag == 2) && systemState != PUMPING) {
          oled_naliv();
          systemON = true;
        }
      }
      if (shotStates[i] != NO_GLASS && !swState) {   // убрали пустую/полную рюмку
        shotStates[i] = NO_GLASS;                    // статус - нет рюмки
        if (!ledShow) {
          strip.setLED(i, mCOLOR(BLACK));  // чёрный
          LEDchanged = true;
        }
        if (i == curPumping ) { // убрали во время заправки!
          systemState = WAIT;                         // режим работы - ждать
          WAITtimer.reset();
          pumpOFF();                                  // помпу выкл
          moving = false;
          play_track(2); //  трек в папке mp3,  кто то снял рюмку при наливе

        }
        //DEBUG("take glass");
        // DEBUG(i);
        yesGlass--;
      }
    }
  }
  if (systemON) flowRoutnie();  // если активны - ищем рюмки и всё такое
}

// поиск и заливка
void flowRoutnie() {
  if (systemState == SEARCH ) {                            // если поиск рюмки
    if (!moving) {
      noGlass = true;
      for (uint8_t i = randFlag; i < NUM_SHOTS; i++) {
        if (MenuFlag == 6 || noDoliv) randFlag++;
        if (shotStates[i] == EMPTY && i != curPumping ) {    // поиск рюмки
          if (MenuFlag == 6) {
            if (subMush == 1 && i == randomGlass) noGlass = false;                           // один за всех
            else if ((subMush == 2 && i != randomGlass) || countRand == 1) noGlass = false;  // все за одного

          } else {
            noGlass = false;
          }

          if (!noGlass) { //  нашли хоть одну рюмку
            if (DrinkCount == 0 && !promivka && MenuFlag != 20) {
              play_track(18); //  трек в папке mp3
            }
            curPumping = i;                                   // запоминаем выбор
            systemState = MOVING;                             // режим - движение
            shotStates[curPumping] = IN_PROCESS;              // стакан в режиме заполнения
            break;
          }
        } else {
          if (MenuFlag == 6 && i <= randomGlass ) randomGlass++;
        }
      }
    }
    if (noGlass) {                                         // если не нашли ни одной рюмки
      servo_move(0);                                        // цель серво - 0
      if (!moving) {                                    // едем до упора
        PAUSEtimer.setInterval(4000);
        PAUSEtimer.reset();
        if (DrinkCount > 0 && !promivka && MenuFlag != 20) {
          oled_nalito();                                 // Выводим на экран налито ...
          tost = true;
          if (player && volume != 0 && tracks != -1 ) {
            playOn = true;
            player = false;
            myMP3.stop();
          } else if (!player && volume != 0 ) {
            myMP3.stop();
          }
          if (ledShowOn) {
            ledShow = true;
#ifdef LED_TOWER
            TOWERtimer.setInterval(20);
            rainbow = true;
#endif
          }
        } else {
          lcd.clear();
          if (yesGlass == 0 || promivka || MenuFlag == 20) {
            lcd.setCursor(3, 1);
            print_lcd(5);//  НЕТ РЮМОК!
          } else {
            lcd.setCursor(1, 1);
            print_lcd(6);//  СНИМИТЕ РЮМКИ!
          }
          returnMenu = true;
          play_track(3); //  трек в папке mp3,  типа предупреждение нет рюмок


#ifdef LED_TOWER
          if (ledShowOn) {
            TOWERtimer.setInterval(50);
            mig = true;
            rainbow = false;
          }
#endif
        }
        DrinkCount = 0;
        systemON = false;
        curPumping = -1; // снимаем выбор рюмки
        //DEBUG("no glass");

      }
    }
  } else if (systemState == MOVING) {                     // движение к рюмке
    if (flag) {
      if (!promivka) oled_naliv();
      else menu_promivka(1);

      flag = false;
    }
    servo_move(shotPos[curPumping]);
    if (!moving) {                                        // если приехали
      systemState = PUMPING;                              // режим - наливание
      if (!promivka) TIMEProcent.setInterval(time50ml / 50); // перенастроили таймер,
      else TIMEProcent.setInterval(1);

      Procent = 0;
      TIMEProcent.reset();
      pumpON();                                           // НАЛИВАЙ!
      if (!promivka && MenuFlag != 20) {
        DrinkCount++;
        lcd.setCursor(11, 0);
        if (MenuFlag == 1 || MenuFlag == 6) lcd.print(Drink, DEC);
        else if (MenuFlag == 2) lcd.print(ManDrink[curPumping], DEC);

        lcd.setCursor(2, 1);
        lcd.print((curPumping + 1), DEC);
        lcd.setCursor(11, 1);
        print_lcd(4);//  пробел 3
      }
      //DEBUG("fill glass");
    }

  } else if (systemState == PUMPING) {                    // если качаем
    if (TIMEProcent.isReady()) {
      Procent++;
      if (!promivka) {
        if ((MenuFlag == 1 || MenuFlag == 20 || MenuFlag == 6) && Procent >= Drink) readyDrink = true;
        else if ( MenuFlag == 2 && Procent >= ManDrink[curPumping]) readyDrink = true;

        lcd.setCursor(12, 1);
        lcd.print(Procent, DEC);
      } else {
        if ( MenuFlag == 11  && (Procent >= 7000 || !enc.isHold())) readyDrink = true;
#ifdef BUTTON_TOWER
        else if (MenuFlag != 11  && (Procent >= 7000  || !stateBut)) readyDrink = true;
#endif

      }
    }
    if (readyDrink) {  // если налили
      pumpOFF();                                          // помпа выкл
      shotStates[curPumping] = READY;                     // налитая рюмка, статус: готов
      strip.setLED(curPumping, mCOLOR(LIME));             // подсветили
      strip.show();
      curPumping = -1;                                    // снимаем выбор рюмки
      systemState = WAIT;                                 // режим работы - ждать
      WAITtimer.reset();
      readyDrink = false;
      //DEBUG("wait");
    }
  } else if (systemState == WAIT) {
    if (WAITtimer.isReady()) {
      systemState = SEARCH;
      //DEBUG("search");
      if (promivka) {
        systemON = false;
        if (MenuFlag == 11) {
          menu_promivka(2);
        } else {
          do {
            servo_move(0);
          } while (moving);
          returnMenu = true;
        }
        promivka = false;
      }
      if (MenuFlag == 20) systemON = false;

    }
  }
}


void servo_move(uint8_t target) {
  static uint32_t prevServoTime = 0;
  static uint8_t pos = 0;
  static bool deadTime = false;
  if (!moving) {
    if (pos != target) {
      moving = true;
      deadTime = false;
      prevServoTime = millis();
    }
  }
  if ( moving && !deadTime && millis() - prevServoTime >= speedSer) {
    prevServoTime = millis();
    if (pos > target) pos--;
    else if (pos < target) pos++;
    write9(pos);
    if ( pos == target ) {
      deadTime = true;
      prevServoTime = millis();
    }
  }

  if (deadTime && millis() - prevServoTime >= POST_PAUSE) { // пауза после остановки сервы до включения помпы
    deadTime = false;
    moving = false;
  }
}

void energy_saving() {
  if (sleepTime != 0 && !systemON && !tost && !save && SAVEtimer.isReady()) {
    play_track(16);
    lcd.noBacklight();
    save = true;
    strip.clear();
    strip.show();
  }
}


void ret_menu() {
  if (returnMenu && PAUSEtimer.isReady()) {
    if (MenuFlag == 1 || MenuFlag == 6) oled_auto();  // выходим в меню автоналива
    else if (MenuFlag == 2) multi_naliv();  // выходим в меню мультиналива
    else if (promivka) menu_promivka(0);
    else if (MenuFlag == 20) kalibr_pump(0);

    returnMenu = false;
    promivka = false;
  }
}

#ifdef BAT_MONITOR_ON
void bat_tery() {
  static uint32_t prevBatTime = 0;
  static uint8_t flagBat = 0;
  static uint8_t flag = 0;
  if (!systemON && !ledShow && !playMush && millis() - prevBatTime >= 1000UL) {
    prevBatTime = millis();
    value = ((value * 4) + analogRead(BAT_PIN) ) / 5;
    if (value < 650) { // 3 вольта 635
      flagBat = 0;
#ifdef LOW_BAT_SLEEP_ON
      if (!lowBat) {
        lowBat = true; // гасим всё
        lcd.noBacklight();
        if (player) {
          playOn = false;
          player = false;
          myMP3.stop();
        }
        strip.clear();
        strip.show();
      }
#endif
    } else if (value < 690) { // 3.2 вольта 677
      flagBat = 1;
    } else if (value < 740) { // 3,4 вольта  720
      flagBat = 2;
#ifdef LOW_BAT_SLEEP_ON
      if (lowBat) {
        lowBat = false;
        lcd.backlight();
        check = true; //  проверяем рюмки
      }
#endif
    } else if (value < 780) { // 3,6 вольта  762
      flagBat = 3;
    } else if (value < 840) { // 3,8 вольта  805
      flagBat = 4;
    } else if (value < 900) { // 4,2 вольта  889
      flagBat = 5;
    } else if (value < 1023) { // 5 вольт от сети
      flagBat = 6;
    }
    if (!lowBat && flagBat != flag && MenuFlag <= 6 && !tost  && !returnMenu) {
      flag = flagBat;
      if (flag == 0) lcd.createChar(7, battery0);
      else if (flag == 1) lcd.createChar(7, battery1);
      else if (flag == 2) lcd.createChar(7, battery2);
      else if (flag == 3) lcd.createChar(7, battery3);
      else if (flag == 4) lcd.createChar(7, battery4);
      else if (flag == 5) lcd.createChar(7, battery5);
      else if (flag == 6) lcd.createChar(7, power);

      lcd.setCursor(15, 0);
      lcd.write(7);
    }
    if (MenuFlag == 18) {
      lcd.setCursor(5, 1);
      lcd.print((value * ADC_U_COEFF), 2);
      //lcd.setCursor(12, 1);  //  раскомментировать, нужно для отладки заряда АКБ
      //lcd.print(value, DEC); //  раскомментировать, нужно для отладки заряда АКБ
    }
  }
}
#endif
void mix() {
  static uint32_t seed;
  if (tracks != -1) {
    for (uint8_t i = 0; i < tracks; i++) TostList[i] = i; // заполняем массив тостов, последовательно значениями
    num = 0;
    if (mixTracks == 1 ) {
      for (uint8_t j = 0; j < 254; j++) {
        seed = 1;
        for (uint8_t i = 0; i < 12; i++) {
          seed *= 4;
          seed += analogRead(SHUM_PIN) & 3;// ловим шумы на аналоговом пине, пин никуда не должен быть подключен
        }
      }
      randomSeed(seed);//  инициализирует генератор псевдослучайных чисел
      for (uint8_t i = tracks - 1; i > 0 ; i--) { // перемешиваем значения случайно в массиве так, что они не повторяются
        uint8_t idx = random( i + 1) ;
        uint8_t t = TostList[i];
        TostList[i] = TostList[idx];
        TostList[idx] = t;
      }
    }
  }
}

void play_next() {
  if (player && !pause && PLAYtimer.isReady() && !nextTrack  ) {
    if (digitalRead(BUSY_PIN)) {
      if (playOn) {
        playOn = false;
        myMP3.volume(volume2);
      }
      nextTrack = true;
      num2++;
    }
  }
  if (nextTrack) {
    PLAYtimer.reset();
    nextTrack = false;
    pause = false;
    oldNum = num2;
    if (num2 <= tracks2) {
      myMP3.playFolder(folder2, num2);
      if (!player) {
        player = true;
        if ( MenuFlag == 3) menu_play(0);
      } else {
        if ( MenuFlag == 3) menu_play(4);
      }
    } else {
      player = false;
      num2 = 1;
      if ( MenuFlag == 3) menu_play(0);
      myMP3.volume(volume);
    }

  }
}

void print_lcd (const uint8_t n) {
  static char buf[32];
  strcpy_P(buf, (char*)pgm_read_word(&(text[n])));
  lcd.print(buf);
}


#ifdef BUTTON_TOWER
void button_tower () {
  static uint32_t butTowTime = 0;
  static uint8_t clickCount = 0;
  static bool lastStateBut = false;
  static bool flagDubl = false;
  if ((MenuFlag == 1 || MenuFlag == 2 || MenuFlag == 6 ) && ((!systemON  && !returnMenu && !save && !playMush) || promivka))  {
    stateBut = digitalRead(BUT_TOWER_PIN);
    if (stateBut != lastStateBut ) {
      lastStateBut = stateBut;
      if (!lastStateBut) {
        if (tost) {
          noTost = true;
        } else {
          clickCount++;
          butTowTime = millis();
          flagDubl = true;
        }
      }

    }
    if (flagDubl &&  millis() - butTowTime >= 400) { // двойной клик на башне
      flagDubl = false;
      if (clickCount >= 2) {
        if (MenuFlag != 6) {
          systemON = true;
          flag = true;
          randFlag = 0;
        } else if (MenuFlag == 6) {
          flagMush = true;
        }
        SAVEtimer.reset();
      }
      clickCount = 0;
    }
  }
}
#endif



void play_track(uint8_t tr) {
  if (!player && volume != 0 ) myMP3.playFromMP3Folder(tr);
}



void play_mushket() {
  if (flagMush) {
    flagMush = false;
    countRand = 0;
    for (uint8_t i = 0; i < NUM_SHOTS; i++) {
      if (shotStates[i] == EMPTY ) countRand++; // подсчёт стопок готовых к наливу
    }
    if (countRand != 0) {
      playMush = true;
      if (ledShowOn) {
        ledShow = true;
#ifdef LED_TOWER
        TOWERtimer.setInterval(20);
        rainbow = true;
#endif
      }
      if (volume != 0 ) {
        if (player) {
          playOn = true;
          player = false;
          myMP3.stop();
          delay(100);
        }
        myMP3.volume(volume);
        delay(100);
        myMP3.playFromMP3Folder(15); // 0015 трек в папке MP3
        lcd.clear();
        lcd.setCursor(2, 0);
        print_lcd(76);//  КРУЧУ ВЕРЧУ
        lcd.setCursor(2, 1);
        print_lcd(77);//  НАЛИТЬ ХОЧУ
        delay(500);
      }
    } else {
      PAUSEtimer.setInterval(4000);
      PAUSEtimer.reset();
      lcd.clear();
      if (yesGlass == 0 ) {
        lcd.setCursor(3, 1);
        print_lcd(5);//  НЕТ РЮМОК!
      } else {
        lcd.setCursor(1, 1);
        print_lcd(6);//  СНИМИТЕ РЮМКИ!
      }
      returnMenu = true;
      play_track(3); //  трек в папке mp3,  типа предупреждение нет рюмок
#ifdef LED_TOWER
      if (ledShowOn) {
        TOWERtimer.setInterval(50);
        mig = true;
        rainbow = false;
      }
#endif
    }
  }

  if (playMush && (digitalRead(BUSY_PIN) || player)) {
    systemON = true;
    flag = true; // флаг ,если пустые рюмки найдены показываем меню налива
    countRand = 0;
    for (uint8_t i = 0; i < NUM_SHOTS; i++) {
      if (shotStates[i] == EMPTY ) countRand++; // снова подсчёт стопок готовых к наливу
    }
    playMush = false;
    randomSeed(micros());
    randomGlass = random(countRand);
    randFlag = 0;
    if (volume != 0 && ledShowOn ) {
      ledShow = false;
      check = true;
#ifdef LED_TOWER
      rainbow = false;
      clearLed = true;
      TOWERtimer.reset();
#endif

    }
  }
}
