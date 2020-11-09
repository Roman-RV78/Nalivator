// кнопки-крутилки

void encTick() {
  if (!returnMenu || promivka) enc.tick();

  if ( !save && !systemON && !tost && !playMush) {
    if (enc.isTurn()) { // если произошло движение энкодера в любую сторону
      SAVEtimer.reset();
      if (enc.isLeft()) drift--;
      if (enc.isRight()) drift++;
      switch (MenuFlag) {
        case 0:    // главное меню
          move_enc(&Menu, drift, 0, 4, true); // Перемещение  по главному меню
          oled_menu();
          break;

        case 1:  // меню автоналива
        case 6:  // меню налива мушкетёров
          move_enc(&Drink, drift * 5, 20, MAX_DRINK, false);
          oled_auto();
          break;

        case 3:  // меню плеера
          if (folTra == 1 ) {
            move_enc(&folder2, drift, 10, 18, false);
            if (player) {
              player = false;
              myMP3.stop();
              menu_play(0);
              delay(100);
            }
            tracks2 = myMP3.numTracksInFolder(folder2);
            if (tracks2 > 100) tracks2 = 100;
            num2 = 0;
            menu_play(1);
          } else if (folTra == 2) {
            move_enc(&num2, drift, 0, tracks2, false);
            menu_play(4);
          } else if (folTra == 3) {
            if (++mixMusic > 1 ) mixMusic = 0;
            menu_play(5);
          } else if (folTra == 4) {
            move_enc(&volume2, drift, 0, 30, false);
            myMP3.volume(volume2);
            menu_play(2);
          }
          break;

        case 4:  // меню настроек
#ifdef BAT_MONITOR_ON
          move_enc(&subNastr, drift, 1, 9, true); // Перемещение  по меню настроек
#else
          move_enc(&subNastr, drift, 1, 8, true); // Перемещение  по меню настроек
#endif
          menu_nastr();
          break;

        case 5:   // меню выбора игры мушкетёров
          move_enc(&subMush, drift, 1, 2, true); // Перемещение  по мушкетёрам
          mushket();
          break;

        case 12: // меню настройки дриньков мультиразлива
          move_enc(&ManDrink[(ManRum - 1)], drift * 5, 20, MAX_DRINK, false);
          multi_naliv2();
          break;

        case 15: // меню настройки яркости led
          move_enc(&bright, drift * 5, 0, 255, false);
          menu_brigh(1);
          strip.setBrightness(bright);
#ifdef LED_TOWER
          strip2.setBrightness(bright);
#endif
          break;

        case 16: // меню включения-выключения бармена
          if (++barMan > 1) barMan = 0;
          bar_man(1);
          break;

        case 19: // меню настройки времени таймера сна
          move_enc(&sleepTime, drift, 0, 19, false);
          sleep_time(1);
          break;

        case 20: //  меню настройки времени налива 50 мл.
          move_enc(&time50ml, drift * 50, 1000, 9950, false);
          kalibr_pump(1);
          break;

        case 30: // меню выбора настройки тостов
          move_enc(&subMenu, drift, 1, 3, true); // Перемещение  по меню тостов
          menu_tost();
          break;

        case 31: //  меню настройки громкости тостов
          move_enc(&volume, drift, 0, 30, false);
          menu_vol(1);
          break;

        case 32: // меню включения-выключения перемешивания тостов
          if (++mixTracks > 1 ) mixTracks = 0;
          mix_track();
          break;

        case 33: // меню выбора папки тостов
          move_enc(&folder, drift, 1, 9, true); // Перемещение  по меню папок
          tracks = myMP3.numTracksInFolder(folder);
          if (tracks > 100) tracks = 100;
          num_folder(1);
          break;

        case 40:  // меню выбора настроек серво
          move_enc(&subMenu, drift, 1, 2, true); // Перемещение  по меню настроек серво
          menu_servo();
          break;

        case 41: // меню настройки позиции серво над рюмками
#ifdef SERVO_CHANGE_DIRECTION
          move_enc(&servoPos, drift, 1, INITAL_ANGLE_SERVO, false);
          servo.write(INITAL_ANGLE_SERVO - servoPos);
#else
          move_enc(&servoPos, drift, 1, 180, false);
          servo.write(servoPos);
#endif
          servo_calibr(2);
          shotPos[count] = servoPos;
          break;

        case 42: // меню настройки скорости серво
          move_enc(&speedSer, -drift, 2, 30, false);
          servo_speed(1);
          break;

      }
      drift = 0;
    }





    if (enc.isDouble()) { // двойной клик кнопки энкодера
      if (MenuFlag == 3 && tracks2 != -1) {
        switch (folTra) {
          case 1: lcd.setCursor(6, 0); break;   //  на выборе папки
          case 2: lcd.setCursor(10, 1); break; // на треках
          case 3: lcd.setCursor(8, 0); break;  //  на рандоме
          case 4: lcd.setCursor(14, 0); break;  //  на громкости
          case 5: lcd.setCursor(15, 1); break;  //  на вкл-выкл play
        }
        print_lcd(79);// пробел
        if (!player) {
          (folTra >= 4 ) ? folTra = 1 : folTra++;
        } else {
          (folTra >= 5 ) ? folTra = 1 : folTra++;
          if (folTra == 3 ) folTra++;
        }

        menu_play(3);
      } else {

        if (player) {
          pause = !pause;
          if (pause) {
            myMP3.pause();
          } else {
            myMP3.resume();
            PLAYtimer.reset();
          }
        }
      }
    }



    if (enc.isHolded()) { // длительное нажатие кнопки энкодера
      if (MenuFlag == 11 && !promivka) {
        promivka = true;
        systemON = true;
        flag = true;
      } else if (MenuFlag != 0 && MenuFlag <= 5) { //Выход в главное меню
        oled_menu();
        if (MenuFlag == 3) { // если были в меню плеера , то обновляем громкость плеера и рандом
#ifdef MEMORY_ON
          address = 100;
          EEPROM.update(address, volume2);
          delay(5);
          address = 110;
          EEPROM.update(address, mixMusic);
#endif
        }
        MenuFlag = 0;
      } else if (MenuFlag == 12) { //Выход из меню настроек мультиразлива в главное меню настроек
        MenuFlag = 4;
        menu_nastr();
#ifdef MEMORY_ON
        address = 10;
        for (uint8_t i = 0; i < NUM_SHOTS; i++) {
          EEPROM.update(address, ManDrink[i]); // обновляем в памяти, дриньки для мультиразлива
          address++;
          delay(5);
        }
#endif
      } else if (MenuFlag == 31) { //Выход из меню громкости в меню тоста
        MenuFlag = 30;
        menu_tost();
        myMP3.volume(volume);
#ifdef MEMORY_ON
        address = 20;
        EEPROM.update(address, volume);
#endif
      } else if (MenuFlag == 32) { //Выход из меню перемешивания  в меню тоста
        MenuFlag = 30;
        menu_tost();
#ifdef MEMORY_ON
        address = 70;
        EEPROM.update(address, mixTracks); // обновляем в памяти флаг перемешивания тостов
#endif
        mix();  // перемешиваем треки, если включено
      } else if (MenuFlag == 33) { //Выход из меню папок  в меню тоста
        MenuFlag = 30;
        menu_tost();
        if (oldFolder != folder ) {
          oldFolder = folder;
          mix();  // перемешиваем треки, если включено
        }
#ifdef MEMORY_ON
        address = 80;
        EEPROM.update(address, folder); // обновляем в памяти выбор папок
#endif
      } else if (MenuFlag == 41) { //Выход из меню  калибровки  сервы в меню настроек сервы
        MenuFlag = 40;
        count = -1;
        servoPos = 0;
        do {
#ifdef STARTING_POS_SERVO_GLASS1
          servo_move(shotPos[0]);                             // цель серво - первая рюмка
#else
          servo_move(0);                                     // цель серво - 0
#endif
        } while (moving);
        menu_servo();
#ifdef MEMORY_ON
        address = 0;
        for (uint8_t i = 0; i < NUM_SHOTS; i++) {
          EEPROM.update(address, shotPos[i]); // обновляем в памяти положение для сервы
          address++;
          delay(5);
        }
#endif
      } else if (MenuFlag == 15) { //Выход из меню настройки яркости led
        MenuFlag = 4;
        menu_nastr();
        check = true;
#ifdef LED_TOWER
        clearLed = true;
        TOWERtimer.reset();
#endif
#ifdef MEMORY_ON
        address = 30;
        EEPROM.put(address, bright); // обновляем в памяти яркость led
#endif
      } else if (MenuFlag == 16) { // выход из режима включения бармена
        MenuFlag = 4;
        menu_nastr();
#ifdef MEMORY_ON
        address = 90;
        EEPROM.update(address, noDoliv); // обновляем в памяти долив
        delay(5);
        address = 120;
        EEPROM.update(address, barMan); // обновляем в памяти режим бармена
#endif
      } else if (MenuFlag == 42) { // выход из настроки скорости серво в меню настройки серво
        MenuFlag = 40;
        menu_servo();
#ifdef MEMORY_ON
        address = 40;
        EEPROM.update(address, speedSer); // обновляем в памяти скорость сервы
#endif
#ifdef BAT_MONITOR_ON
      } else if (MenuFlag == 18) {  // выход из меню просмотра напряжения батареи в меню настроек
        MenuFlag = 4;
        menu_nastr();
#endif
      } else if (MenuFlag == 19) {  // выход из меню настройки таймера сна в менюнастроек
        MenuFlag = 4;
        menu_nastr();
#ifdef MEMORY_ON
        address = 50;
        EEPROM.update(address, sleepTime); // обновляем в памяти таймер сна
#endif
        if (sleepTime != 0) SAVEtimer.setInterval(sleepTime * 30000UL);

      } else if (MenuFlag == 20) {  // выход из меню настройки помпы в меню настроек
        MenuFlag = 4;
        do {
#ifdef STARTING_POS_SERVO_GLASS1
          servo_move(shotPos[0]);                             // цель серво - первая рюмка
#else
          servo_move(0);                                     // цель серво - 0
#endif
        } while (moving);
        menu_nastr();
#ifdef MEMORY_ON
        address = 60;
        EEPROM.put(address, time50ml); // обновляем в памяти время налива 50 мл.
#endif
      } else if (MenuFlag == 30 || MenuFlag == 40) { // выход из меню настройки тостов и серво в меню настроек
        MenuFlag = 4;
        menu_nastr();
        if (playOn) player = true;

      } else if (MenuFlag == 6) { // выход из меню розлива мушкетёров  в меню мушкетёров
        MenuFlag = 5;
        mushket();
      }
    }
  }

  if ( enc.isSingle()) { // одиночный клик
    SAVEtimer.reset();
    if (save) {
      play_track(17);
      save = false;
      lcd.backlight();
      check = true; //  проверяем рюмки
    } else {
      if (systemON || playMush ) {  // нажата кнопка во время работы наливатора, то отбой всего
        systemState = SEARCH;
        DrinkCount = 0;
        shotStates[curPumping] = READY;
        curPumping = -1; // снимаем выбор рюмки
        systemON = false;
        playMush = false;
        check = true;
        pumpOFF();                // помпу выкл
        moving = false;
        if (ledShowOn) {
          ledShow = false;
#ifdef LED_TOWER
          clearLed = true;
          rainbow = false;
          mig = false;
          TOWERtimer.reset();
#endif
        }
        lcd.clear();
        lcd.setCursor(5, 1);
        print_lcd(70);//  ОТБОЙ!
        play_track(4); //  трек в папке mp3,

        do {
#ifdef STARTING_POS_SERVO_GLASS1
          servo_move(shotPos[0]);                             // цель серво - первая рюмка
#else
          servo_move(0);                                     // цель серво - 0
#endif
        } while (moving);
        delay(3000);
        if (playOn) {
          player = true;
          PLAYtimer.reset();
          pause = false;
        }
        returnMenu = true;
      } else if (tost ) { // если во время произношения тоста нажата кнопка, то отменяем воспроизведение
        if ( !digitalRead(BUSY_PIN)) {
          noTost = true;
          if (ledShowOn) {
            ledShow = false;
            check = true;
#ifdef LED_TOWER
            rainbow = false;
            clearLed = true;
            TOWERtimer.reset();
#endif
          }
        }
      } else if (Menu == 0 && MenuFlag == 0) { //Нажатие кнопки меню авто
        MenuFlag = 1;
        oled_auto();
      } else if (MenuFlag == 1 || MenuFlag == 2 ) { //Начинается автоматический разлив
        if (barMan == 0 && readySystem) {
          systemON = true;
          flag = true; // флаг ,если пустые рюмки найдены показываем меню налива
          if (noDoliv == 1) readySystem = false;
#ifdef BUTTON_TOWER
          if ( stateBut) promivka = true;
#endif
          if (ledShowOn) {
            if (ledShow) check = true;
#ifdef LED_TOWER
            TOWERtimer.setInterval(50);
            if (mig || rainbow ) {
              mig = false;
              rainbow = false;
              clearLed = true;
              TOWERtimer.reset();

            }
#endif
          }
        }
      } else if (Menu == 1 && MenuFlag == 0) { // меню мультиразлив
        MenuFlag = 2;
        multi_naliv();
      } else if (Menu == 2 && MenuFlag == 0) { // меню плеера
        if (!player) {
          tracks2 = myMP3.numTracksInFolder(folder2);
          if (tracks2 > 100) tracks2 = 100;
        }
        if (tracks2 == -1) folTra = 1; // галочка на выборе папок
        MenuFlag = 3;
        menu_play(0);
      } else if (Menu == 2 && MenuFlag == 3) { // вошли в меню плеера
        if (!player && tracks2 != -1) {
          mix_music();
          nextTrack = true;
          myMP3.volume(volume2);
          folTra = 2;
        } else {
          if (folTra == 2) {
            if (oldNum == num2) num2++;
            nextTrack = true;
          } else if (folTra == 5) {
            player = false;
            myMP3.stop();
            folTra = 1;
            menu_play(0);
            delay(100);
            myMP3.volume(volume);
          }
        }
      } else if (Menu == 3 && MenuFlag == 0) { // меню настроек
        MenuFlag = 4;
        menu_nastr();
      } else if (Menu == 3 && MenuFlag == 4) { // вошли в меню настроек
        switch (subNastr) {
          case 1:  // промывка
            MenuFlag = 11;
            menu_promivka(0);
            break;
          case 2:  //мультиразлив
            MenuFlag = 12;
            multi_naliv2();
            break;
          case 3:  //   меню тосты
            MenuFlag = 30;
            if (player) {
              player = false;
              pause = false;
              myMP3.stop();
              playOn = true;
            }
            subMenu = 1;
            menu_tost();
            break;
          case 4:  // серво
            MenuFlag = 40;
            subMenu = 1;
            menu_servo();
            break;
          case 5:  // яркость led
            MenuFlag = 15;
            menu_brigh(0);
            break;
          case 6:  // режим бармен
            MenuFlag = 16;
            bar_man(0);
            break;
          case 7:  // калибровка помпы
            MenuFlag = 20;
            kalibr_pump(0);
            break;
          case 8:  // таймер сна
            MenuFlag = 19;
            sleep_time(0);
            break;
#ifdef BAT_MONITOR_ON
          case 9:  // напряжение батареи
            MenuFlag = 18;
            bat_volt();
            break;
#endif
        }
      } else if (Menu == 4 && MenuFlag == 0) { // меню мушкетёры
        MenuFlag = 5;
        mushket();
      } else if (Menu == 4 && MenuFlag == 5) { // вошли в меню мушкетёры
        //barMan = 0;
        MenuFlag = 6;
        oled_auto();
      } else if (MenuFlag == 6 ) { // начинаем розлив мушкетёров
        if (readySystem) flagMush = true;
      } else if (MenuFlag == 30 && subMenu == 1 ) { // вошли из меню тост в настройку громкости
        MenuFlag = 31;
        menu_vol(0);
      } else if (MenuFlag == 30 && subMenu == 2 ) {  // вошли из меню тост в настройку перемешивания треков
        MenuFlag = 32;
        mix_track();

      } else if (MenuFlag == 30 && subMenu == 3 ) {  // вошли из меню тост в настройку   папки проигрывания
        MenuFlag = 33;
        num_folder(0);
      } else if (MenuFlag == 40 && subMenu == 1 ) {  // вошли из меню сервы в настройку положения сервы над рюмками
        MenuFlag = 41;
        servo_calibr(0);
      } else if (MenuFlag == 40 && subMenu == 2 ) {  // вошли из меню сервы в настройку  скорости сервы
        MenuFlag = 42;
        servo_speed(0);

      } else if (MenuFlag == 11 ) { // меню настройки промывка
        do {
#ifdef STARTING_POS_SERVO_GLASS1
          servo_move(shotPos[0]);                             // цель серво - первая рюмка
#else
          servo_move(0);                                     // цель серво - 0
#endif
        } while (moving);
        menu_nastr();
        MenuFlag = 4;
      } else if (MenuFlag == 12 ) { // меню настройки мультиналива
        if (++ManRum > 6) ManRum = 1;
        multi_naliv2();
      } else if (MenuFlag == 15 ) { // меню настройки яркости
        ledShowOn = !ledShowOn;
        menu_brigh(2);
      } else if (MenuFlag == 16 ) { // меню настройки долив-бармен
        if (++noDoliv > 1) noDoliv = 0;
        bar_man(2);
      } else if (MenuFlag == 31) { //меню настройки громкости
        if (volume != 0) tost = true;  // проверяем громкость

      } else if (MenuFlag == 42) {  // двигаем серво туда-обратно для наглядности установленной скорости
        do {
          servo_move(180);
        } while (moving);

        do {
#ifdef STARTING_POS_SERVO_GLASS1
          servo_move(shotPos[0]);                             // цель серво - первая рюмка
#else
          servo_move(0);                                     // цель серво - 0
#endif
        } while (moving);

      } else if (MenuFlag == 20) {  // наливаем в рюмку для проверки величины налива, после изменения времени налива
        Drink = 50;
        systemON = true;
        kalibr_pump(1);
      }


      if (MenuFlag == 41 ) { //Калибровка серво
        if (++count >= NUM_SHOTS) count = 0;

        servoPos = shotPos[count];
        servo_calibr(1);
        do {
          servo_move(servoPos);
        } while (moving);
      }
    }

  }
}
