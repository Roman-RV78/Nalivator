void oled_menu() {
  lcd.clear();
  lcd.setCursor(3, 0);
  print_lcd(7);    // НАЛИВАТОР+
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  lcd.setCursor(0, 1);
  print_lcd(66);// >
  lcd.setCursor(15, 1);
  print_lcd(65);// <
  switch (Menu) {
    case 0:
      lcd.setCursor(6, 1);
      print_lcd(8);//  АВТО
      break;
    case 1:
      lcd.setCursor(2, 1);
      print_lcd(9);//  МУЛЬТИРАЗЛИВ
      break;
    case 2:
      lcd.setCursor(5, 1);
      print_lcd(10);//  ПЛЕЕР
      break;
    case 3:
      lcd.setCursor(3, 1);
      print_lcd(11);//  НАСТРОЙКИ
      break;

    case 4:
      lcd.setCursor(3, 1);
      print_lcd(71);//  МУШКЕТЁРЫ
      break;
  }
}



void menu_nastr() {
  lcd.clear();
  lcd.setCursor(3, 0);
  print_lcd(11);//  НАСТРОЙКА
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  lcd.setCursor(0, 1);
  print_lcd(66);//  >
  lcd.setCursor(15, 1);
  print_lcd(65);//  <
  switch (subNastr) {
    case 1:
      lcd.setCursor(4, 1);
      print_lcd(12);// ПРОМЫВКА
      break;
    case 2:
      lcd.setCursor(2, 1);
      print_lcd(9);// МУЛЬТИРАЗЛИВ
      break;
    case 3:
      lcd.setCursor(5, 1);
      print_lcd(13);//  ТОСТЫ
      break;

    case 4:
      lcd.setCursor(2, 1);
      print_lcd(14);//  МЕНЮ  СЕРВО
      break;

    case 5:
      lcd.setCursor(2, 1);
      print_lcd(15);//  ЯРКОСТЬ  LED
      break;

    case 6:
      lcd.setCursor(2, 1);
      print_lcd(16);//  БАРМЕН-ДОЛИВ
      break;


    case 7:
      lcd.setCursor(2, 1);
      print_lcd(19);// КАЛИБР.ПОМПЫ
      break;

    case 8:
      lcd.setCursor(3, 1);
      print_lcd(18);//  ТАЙМЕР СНА
      break;
#ifdef BAT_MONITOR_ON
    case 9:
      lcd.setCursor(1, 1);
      print_lcd(17);//  НАПРЯЖЕНИЕ АКБ
      break;
#endif
  }
}



// Меню Авто режим
void oled_auto() {
  lcd.clear();
  lcd.setCursor(0, 0);
  print_lcd(20);//  HАЛИTЬ
  lcd.setCursor(7, 0);
  print_lcd(29);//  ПО    ml
  lcd.setCursor(10, 0);
  lcd.print(Drink, DEC);
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  lcd.setCursor(0, 1);
  if (Drink < 25) print_lcd(21);//  HИ O ЧEM
  else if (Drink < 30) print_lcd(22);//  ПO ЧУTЬ - ЧУTЬ
  else if (Drink < 40) print_lcd(23);//  B CAMЫЙ PAЗ
  else if (Drink < 45) print_lcd(24);//  ПО  ПОЛНОЙ
  else  print_lcd(25);//  ДО КРАЕВ

  if (barMan == 1) {
    lcd.setCursor(15, 1);
    print_lcd(38);//  B
  }
  if (MenuFlag == 6) {
    lcd.setCursor(15, 1);
    print_lcd(74);//  M
  }
}

// Меню налива
void oled_naliv() {
  lcd.clear();
  lcd.setCursor(2, 0);
  print_lcd(26);//  НАЛИВАЮ     ml
  lcd.setCursor(0, 1);
  print_lcd(27);//  В  -Ю РЮМКУ   ml
}

// Меню налито
void oled_nalito() {
  lcd.clear();
  if (MenuFlag == 2) {
    lcd.setCursor(5, 0);
  } else {
    lcd.setCursor(7, 0);
    print_lcd(29);//  ПО    ml
    lcd.setCursor(10, 0);
    lcd.print(Drink, DEC);
    lcd.setCursor(0, 0);
  }
  print_lcd(28); //  HАЛИТО
  lcd.setCursor(0, 1);
  print_lcd(30);//    В   РЮМ
  lcd.setCursor(5, 1);
  lcd.print(DrinkCount, DEC);
  lcd.setCursor(10, 1);
  if (DrinkCount == 1) print_lcd(31);//  КУ
  else if (DrinkCount <= 4 ) print_lcd(32);//  КИ
  else print_lcd(33);//  ОК

}

// Меню настройки серво
void servo_calibr(uint8_t subServo) {
  if (subServo == 0) {
    lcd.clear();
    lcd.setCursor(4, 0);
    print_lcd(34);//  -я рюмка
    lcd.setCursor(3, 1);
    print_lcd(35);//  поз.:
  }
  if (subServo == 1 ) {
    lcd.setCursor(3, 0);
    lcd.print((count + 1), DEC);
  }
  if (subServo == 2 ||  subServo == 1) {
    lcd.setCursor(10, 1);
    print_lcd(4);//   пробел 3
    lcd.setCursor(9, 1);
    lcd.print(servoPos, DEC);
  }
}


void multi_naliv() {
  lcd.clear();
  lcd.setCursor(0, 0);
  print_lcd(36);//  1-   2-   3-
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  lcd.setCursor(2, 0);
  lcd.print(ManDrink[0], DEC);
  lcd.setCursor(7, 0);
  lcd.print(ManDrink[1], DEC);
  lcd.setCursor(12, 0);
  lcd.print(ManDrink[2], DEC);
  lcd.setCursor(0, 1);
  print_lcd(37);//   4-   5-   6-
  lcd.setCursor(2, 1);
  lcd.print(ManDrink[3], DEC);
  lcd.setCursor(7, 1);
  lcd.print(ManDrink[4], DEC);
  lcd.setCursor(12, 1);
  lcd.print(ManDrink[5], DEC);
  if (barMan == 1) {
    lcd.setCursor(15, 1);
    print_lcd(38);//  B
  }
}

void multi_naliv2() {
  lcd.clear();
  lcd.setCursor(0, 1);
  if (ManDrink[(ManRum - 1)] < 25) print_lcd(21);//  НИ О ЧЕМ
  else if (ManDrink[(ManRum - 1)] < 30) print_lcd(22);//  ПО ЧУТЬ - ЧУТЬ
  else if (ManDrink[(ManRum - 1)] < 40) print_lcd(23);//  В САМЫЙ  РАЗ
  else if (ManDrink[(ManRum - 1)] < 45) print_lcd(24);//  ПО  ПОЛНОЙ
  else print_lcd(25);//  ДО КРАЕВ

  lcd.setCursor(0, 0);
  lcd.print(ManRum, DEC);
  lcd.setCursor(1, 0);
  print_lcd(39);// -Я РЮМКА:   ml
  lcd.setCursor(10, 0);
  lcd.print(ManDrink[(ManRum - 1)], DEC);
}

void menu_tost() {
  lcd.clear();
  lcd.setCursor(5, 0);
  print_lcd(13);// ТОСТЫ
  switch (subMenu) {
    case 1:
      lcd.setCursor(3, 1);
      print_lcd(40);// ГР.ОЗВУЧКИ
      break;

    case 2:
      lcd.setCursor(0, 1);
      print_lcd(41);//  ПЕРЕМЕШАТЬ ТРЕКИ
      break;

    case 3:
      lcd.setCursor(0, 1);
      print_lcd(42);//  ПАПКА С ТРЕКАМИ
      break;

  }
}

void num_folder(uint8_t subFolder) {
  if (subFolder == 0) {
    lcd.clear();
    lcd.setCursor(3, 0);
    print_lcd(43);//  ПАПКА
    lcd.setCursor(2, 1);
    print_lcd(44);//  ТРЕКОВ
  }
  if (subFolder == 1 || subFolder == 0) {
    lcd.setCursor(11, 0);
    lcd.print(folder, DEC);
    lcd.setCursor(11, 1);
    if (tracks == -1) {
      print_lcd(45); //  НЕТ
    } else {
      if ( subFolder != 0) print_lcd(4); // пробел 3
      lcd.setCursor(11, 1);
      lcd.print(tracks, DEC);
    }

  }
}
void menu_play(uint8_t subPlay) {
  if (subPlay == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    print_lcd(43);//  ДИР:
    lcd.setCursor(9, 0);
    print_lcd(67); // VOL;
#ifdef BAT_MONITOR_ON
    lcd.setCursor(15, 0);
    lcd.write(7);
#endif
    lcd.setCursor(0, 1);
    print_lcd(68);//  ТР:
    lcd.setCursor(11, 1);
    if (player) print_lcd(46);//  PLAY
  }
  if (subPlay == 1 || subPlay == 0 ) {
    lcd.setCursor(4, 0);
    lcd.print(folder2, DEC);
    lcd.setCursor(3, 1);
    if (tracks2 == -1) {
      print_lcd(45);// НЕТ
    } else {
      if (subPlay != 0) {
        print_lcd(4);// пробел
      }
      lcd.setCursor(3, 1);
      lcd.print(tracks2, DEC);
    }
  }
  if (subPlay == 2 || subPlay == 0 ) {
    if (subPlay != 0) {
      lcd.setCursor(13, 0);
      print_lcd(79);
    }
    lcd.setCursor(12, 0);
    lcd.print(volume2, DEC);
  }
  if (subPlay == 3 || subPlay == 0 ) {
    switch (folTra) {
      case 1: lcd.setCursor(6, 0); break;   // галочка на выборе папки
      case 2: lcd.setCursor(10, 1); break; // галочка на треках
      case 3: lcd.setCursor(8, 0); break;  //  галочка на вкл-выкл рандом
      case 4: lcd.setCursor(14, 0); break;  // галочка на громкости
      case 5: lcd.setCursor(15, 1); break;  //  галочка на вкл-выкл play
    }
    print_lcd(65);//  <
  }
  if (subPlay == 4 || subPlay == 1 || subPlay == 0 ) {
    if (subPlay != 0) {
      lcd.setCursor(7, 1);
      print_lcd(4);// пробел
    }
    if (tracks2 != -1) {
      lcd.setCursor(7, 1);
      lcd.print((MusicList[num2] + 1), DEC);
    }
  }
  if (subPlay == 5 || subPlay == 0 ) {
    lcd.setCursor(7, 0);
    if (mixMusic == 1 ) print_lcd(80);// R
    else print_lcd(81);  // L

  }
}



void menu_servo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  print_lcd(47);//  НАСТРОЙКИ  СЕРВО
  switch (subMenu) {
    case 1:
      lcd.setCursor(3, 1);
      print_lcd(48);//  КАЛИБРОВКА
      break;

    case 2:
      lcd.setCursor(4, 1);
      print_lcd(49);//  СКОРОСТЬ
      break;

  }
}


void menu_vol(uint8_t subVol) {
  if (subVol == 0) {
    lcd.clear();
    lcd.setCursor(0, 1);
    print_lcd(50);//  ГРОМКОСТЬ
  }
  if (subVol == 1 || subVol == 0 ) {
    if (subVol != 0) {
      lcd.setCursor(12, 1);
      print_lcd(4);//  пробел 3
    }
    lcd.setCursor(13, 1);
    lcd.print(volume, DEC);
  }
}


void menu_brigh(uint8_t subBrigh) {
  if (subBrigh == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    print_lcd(51);//  ЯРКОСТЬ LED
    lcd.setCursor(10, 0);
    print_lcd(69);//  SHOW
  }
  if (subBrigh == 1 || subBrigh == 0) {
    lcd.setCursor(2, 1);
    print_lcd(4);// пробел 3
    lcd.setCursor(2, 1);
    lcd.print((map(bright, 0, 254, 0, 99)), DEC);
  }
  if (subBrigh == 2 || subBrigh == 0) {
    lcd.setCursor(10, 1);
    if (ledShowOn) print_lcd(54);// ВКЛ.
    else print_lcd(55);// ВЫКЛ.
  }
}

void menu_promivka(uint8_t subPromivka) {
  lcd.clear();
  lcd.setCursor(0, 0);
  print_lcd(52);//  П Р О М Ы В К А
  if (subPromivka == 1) {
    lcd.setCursor(2, 1);
    print_lcd(53);//  >>>>>>>>>>>>
  } else if (subPromivka == 2) {
    lcd.setCursor(5, 1);
    lcd.print(Procent, DEC);
    lcd.setCursor(10, 1);
    print_lcd(63);// мс
  }
}

void bar_man(uint8_t subBarmen) {
  if (subBarmen == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    print_lcd(78);// БАРМЕН
    lcd.setCursor(6, 0);
    print_lcd(77);// ДОЛИВ
    lcd.setCursor(11, 0);
    print_lcd(13);// ТОСТЫ
  }
  if (subBarmen == 1 || subBarmen == 0) {
    lcd.setCursor(0, 1);
    if (barMan == 1) print_lcd(54);// ВКЛ.
    else print_lcd(55);// ВЫКЛ.
  }
  if (subBarmen == 2 || subBarmen == 0) {
    lcd.setCursor(6, 1);
    if (noDoliv == 1) print_lcd(55);// ВЫКЛ.
    else print_lcd(54);// ВКЛ
  }
  if (subBarmen == 3 || subBarmen == 0) {
    lcd.setCursor(11, 1);
    if (noTostBarmen == 1) print_lcd(55);// ВЫКЛ.
    else print_lcd(54);// ВКЛ
  }
}

void mix_track() {
  lcd.clear();
  lcd.setCursor(6, 1);
  if (mixTracks == 1) print_lcd(54);// ВКЛ.
  else print_lcd(55);// ВЫКЛ.
}

void servo_speed(uint8_t subSS) {
  if (subSS == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    print_lcd(49);//  СКОРОСТЬ
  }
  if (subSS == 1 || subSS == 0) {
    if (subSS != 0) {
      lcd.setCursor(12, 0);
      print_lcd(4);// пробел 3
    }
    lcd.setCursor(12, 0);
    lcd.print((map(speedSer, 2, 30, 30, 2)), DEC);
  }
}
#ifdef BAT_MONITOR_ON
void bat_volt() {
  lcd.clear();
  lcd.setCursor(3, 0);
  print_lcd(56);//  НАПРЯЖЕНИЕ
  lcd.setCursor(5, 1);
  lcd.print((value * ADC_U_COEFF), 2);
  lcd.setCursor(10, 1);
  print_lcd(57);// V
}
#endif

void sleep_time(uint8_t subTime) {
  if (subTime == 0) {
    lcd.clear();
    lcd.setCursor(1, 0);
    print_lcd(58);//  ЗАСНУТЬ ЧЕРЕЗ
  }
  if (subTime == 1 || subTime == 0) {
    if (sleepTime == 0) {
      if (subTime != 0) {
        lcd.setCursor(10, 1);
        print_lcd(4);//  пробел 3
      }
      lcd.setCursor(5, 1);
      print_lcd(55);//  ВЫКЛ.
    } else {
      lcd.setCursor(5, 1);
      lcd.print((map(sleepTime, 0, 20, 0, 10)), DEC);
      lcd.setCursor(6, 1);
      if (sleepTime % 2 != 0) print_lcd(59);//  .30
      else print_lcd(60);//  .00

      lcd.setCursor(9, 1);
      print_lcd(61);//  МИН.
    }
  }
}


void kalibr_pump(uint8_t subPump) {
  if (subPump == 0) {
    lcd.clear();
    lcd.setCursor(1, 0);
    print_lcd(62);//  НАЛИВ 50ml ЗА:
    lcd.setCursor(8, 1);
    print_lcd(63);//  мс
  }
  if (subPump == 1 || subPump == 0) {
    lcd.setCursor(3, 1);
    lcd.print(time50ml, DEC);
    lcd.setCursor(12, 1);
    print_lcd(64);//  ml
  }
}



void mushket() {
  lcd.clear();
  lcd.setCursor(3, 0);
  print_lcd(71);//  МУШКЕТЁРЫ
#ifdef BAT_MONITOR_ON
  lcd.setCursor(15, 0);
  lcd.write(7);
#endif
  switch (subMush) {
    case 1:
      lcd.setCursor(2, 1);
      print_lcd(72);// ОДИН ЗА ВСЕХ
      break;
    case 2:
      lcd.setCursor(1, 1);
      print_lcd(73);// ВСЕ ЗА ОДНОГО
      break;
  }
}
