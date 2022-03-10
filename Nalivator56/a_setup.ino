void setup() {
  strip.clear();
  strip.show();
#ifdef LED_TOWER
  strip2.clear();
  strip2.show();
#endif

#ifdef BAT_MONITOR_ON
#if  defined(__AVR_ATmega328P__)
  analogReference(INTERNAL);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  analogReference(INTERNAL1V1);
#endif
#endif


  // настройка пинов
  pinMode(PUMP1_POWER, OUTPUT);
#if (PUMP_LEVEL == 0)
  digitalWrite(PUMP1_POWER, HIGH);
#ifdef TWO_PUMPS
  pinMode(PUMP2_POWER, OUTPUT);
  digitalWrite(PUMP2_POWER, HIGH);
#endif
#else
  digitalWrite(PUMP1_POWER, LOW);
#ifdef TWO_PUMPS
  pinMode(PUMP2_POWER, OUTPUT);
  digitalWrite(PUMP2_POWER, LOW);
#endif
#endif

  for (uint8_t i = 0; i < NUM_SHOTS; i++) {
#if(SWITCH_LEVEL == 0)
    pinMode(SW_pins[i], INPUT_PULLUP);
#elif(SWITCH_LEVEL == 1)
    pinMode(SW_pins[i], INPUT);
#endif
  }
  pinMode(BUSY_PIN, INPUT_PULLUP);

#ifdef BUTTON_TOWER
  pinMode(BUT_TOWER_PIN, INPUT);
#endif
#ifdef SEVE_MODE_CONDITION
  pinMode(SEVE_MODE_PIN, OUTPUT);
  digitalWrite(SEVE_MODE_PIN, HIGH);
#endif


  Procent = 1;
  for (uint8_t i = 0; i < NUM_SHOTS; i++) {
    EEPROM.get(address, shotPos[i]); // считываем из памяти положение для сервы
    if ( shotPos[i] < 1 ||  shotPos[i] > 180) shotPos[i] = Procent;  // если ячейки памяти не в интервале, то ставим начальные значения
    address++;
    Procent += (180 / (NUM_SHOTS - 1)) - 1;
  }

  address = 10;
  for (uint8_t i = 0; i < NUM_SHOTS; i++) {
    EEPROM.get(address, ManDrink[i]); // считываем из памяти, дриньки для мультиразлива
    if ( ManDrink[i] < MIN_DRINK ||  ManDrink[i] > MAX_DRINK) ManDrink[i] = 20; // если ячейки памяти не в интервале, то ставим начальные значения
    address++;
  }
  address = 20;
  EEPROM.get(address, volume); // считываем из памяти громкость
  if ( volume > 30) volume = 10;

  address = 30;
  EEPROM.get(address, bright); // считываем из памяти яркость led

  address = 40;
  EEPROM.get(address, speedSer); // считываем из памяти скорость сервы
  if (speedSer < 2 || speedSer > 30) speedSer = 10;

  address = 50;
  EEPROM.get(address, sleepTime); // считываем из памяти время таймера сна
  if ( sleepTime > 20) sleepTime = 0;
  else SAVEtimer.setInterval(sleepTime * 30000UL);

  address = 60;
  EEPROM.get(address, time50ml_1); // считываем из памяти время налива 50 мл для первой помпы.
  if (time50ml_1 < 1000 || time50ml_1 > 10000) time50ml_1 = 5000;
#ifdef TWO_PUMPS
  address = 150;
  EEPROM.get(address, time50ml_2); // считываем из памяти время налива 50 мл для второй помпы.
  if (time50ml_2 < 1000 || time50ml_2 > 10000) time50ml_2 = 5000;
#endif
  address = 70;
  EEPROM.get(address, mixTracks); // считываем из памяти флаг перемешивания тостов
  if ( mixTracks > 1) mixTracks = 0;

  address = 80;
  EEPROM.get(address, folder); // считываем из памяти номер папки треков тостов
  if (folder < 1 || folder > 9) folder = 1;
  oldFolder = folder;

  address = 90;
  EEPROM.get(address, noDoliv); // считываем из памяти флаг долива
  if ( noDoliv > 1) noDoliv = 1;

  address = 100;
  EEPROM.get(address, volume2); // считываем из памяти громкость для плеера
  if ( volume2 > 30) volume2 = 10;

  address = 110;
  EEPROM.get(address, mixMusic); // считываем из памяти флаг перемешивания песенок
  if ( mixMusic > 1) mixMusic = 1;

  address = 120;
  EEPROM.get(address, ledShowOn); // считываем из памяти флаг включения иллюминации стола
  if ( ledShowOn > 1) ledShowOn = 1;
  if ( ledShowOn == 1) {
    ledShow = true;
#ifdef LED_TOWER
    rainbow = true;
#endif
  } else {
    LEDtimer.setInterval(GLASS_RAINBOW_ILLUMINATION_TIMER);
  }

  address = 130;
  EEPROM.get(address, noTostBarmen); // считываем из памяти флаг тостов в  бармене
  if ( noTostBarmen > 1) noTostBarmen = 1;
#ifdef TWO_PUMPS
  address = 140;
  for (uint8_t i = 0; i < NUM_SHOTS; i++) {
    EEPROM.get(address, pourFrom[i]); // считываем из памяти номер разливного крана, если включена опция двух кранов
    if ( pourFrom[i] > 1) pourFrom[i] = 0; // если ячейки памяти не в интервале, то ставим начальные значения
    address++;
  }
#endif

#ifdef BAT_MONITOR_ON
  address = 160;
  EEPROM.get(address, coeff_bat); // считываем из памяти коэффициент калибровки АКБ
  if (coeff_bat < ADC_U_COEFF_HIGH && coeff_bat > ADC_U_COEFF_LOW );//норма
  else coeff_bat = ADC_U_COEFF; //NaN
#endif

  Procent = 0;

#if (DEBUG_UART == 1)
  Serial.begin(9600);
#endif

  myMP3.begin(9600, PLAYER_SERIAL_TIMEOUT); // скорость порта, таймаут порта мс.
  delay (1000);
  myMP3.setEq(DfMp3_Eq_Normal);
  delay (100); //Между двумя командами необходимо делать задержку 100 миллисекунд, в противном случае некоторые команды могут работать не стабильно.
  tracks = myMP3.getFolderTrackCount(folder); // считываем количество треков в папке 01-09 в корне флешки, не больше 100
  if (tracks > 100) tracks = 100;
  delay (100);
  myMP3.setVolume(volume);
  delay (100);
#if(SOUND_THEME == 1)
  play_track(50);  //танк завёлся
#else
  play_track(1);  // Проигрываем "mp3/0001.mp3"(0001_get started!.mp3)
#endif
  lcd.init();// Инициализация дисплея
  lcd.backlight();
  lcd.setCursor(7, 0);
  print_lcd(0);// НУ,
  lcd.setCursor(1, 1);
  print_lcd(1);//  НАА-ЧАА-ЛИИИИ!
  strip.setBrightness(bright);
#ifdef LED_TOWER
  strip2.setBrightness(bright);
#endif
  //servo.attach(SERVO_PIN);
  servo.attach(SERVO_PIN, SERVO_MIN, SERVO_MAX);
#ifdef STARTING_POS_SERVO_GLASS1
#ifdef SERVO_CHANGE_DIRECTION
  servo.write(INITAL_ANGLE_SERVO - shotPos[0]);
#else
  servo.write(shotPos[0]);
#endif
#else
#ifdef SERVO_CHANGE_DIRECTION
  servo.write(INITAL_ANGLE_SERVO);
#else
  servo.write(0);
#endif
#endif
  delay(2000);
  oled_menu();
  mix();
}
