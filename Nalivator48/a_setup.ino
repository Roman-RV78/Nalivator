void setup() {
  strip.clear();
  strip.show();
#ifdef LED_TOWER
  strip2.clear();
  strip2.show();
#endif
  delay(1000);
#if (DEBUG_UART == 1)
  Serial.begin(9600);
#endif
#ifdef BAT_MONITOR_ON
#if  defined(__AVR_ATmega328P__)
  analogReference(INTERNAL);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  analogReference(INTERNAL1V1);
#endif


#endif
#if  defined(__AVR_ATmega328P__)
  mySerial.begin (9600);
  delay (100);
  myMP3.begin(mySerial); //инициализируем Serial порт МП3 плейера
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  Serial3.begin(9600);
  delay (100);
  myMP3.begin(Serial3); //инициализируем Serial порт МП3 плейера 14 и 15 пины меги
#endif
  // настройка пинов
  pinMode(PUMP_POWER, OUTPUT);
#if (PUMP_LEVEL == 0)
  digitalWrite(PUMP_POWER, HIGH);
#else
  digitalWrite(PUMP_POWER, LOW);
#endif

  for (uint8_t i = 0; i < NUM_SHOTS; i++) {
#if(SWITCH_LEVEL == 0)
    pinMode(SW_pins[i], INPUT_PULLUP);
#elif(SWITCH_LEVEL == 1)
    pinMode(SW_pins[i], INPUT);
#endif
  }
  pinMode(BUSY_PIN, INPUT);

#ifdef BUTTON_TOWER
  pinMode(BUT_TOWER_PIN, INPUT);
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
    if ( ManDrink[i] < 20 ||  ManDrink[i] > MAX_DRINK) ManDrink[i] = 20; // если ячейки памяти не в интервале, то ставим начальные значения
    address++;
  }
  address = 20;
  EEPROM.get(address, volume); // считываем из памяти громкость
  if ( volume < 0 || volume > 30) volume = 10;

  address = 30;
  EEPROM.get(address, bright); // считываем из памяти яркость led
  if ( bright < 0 || bright > 255) bright = 100;

  address = 40;
  EEPROM.get(address, speedSer); // считываем из памяти скорость сервы
  if (speedSer < 2 || speedSer > 30) speedSer = 10;

  address = 50;
  EEPROM.get(address, sleepTime); // считываем из памяти время таймера сна
  if (sleepTime <= 0 || sleepTime > 20) sleepTime = 0;
  else SAVEtimer.setInterval(sleepTime * 30000UL);

  address = 60;
  EEPROM.get(address, time50ml); // считываем из памяти время налива 50 мл.
  if (time50ml < 1000 || time50ml > 10000) time50ml = 5000;

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
  if ( volume2 < 0 || volume2 > 30) volume2 = 10;

  address = 110;
  EEPROM.get(address, mixMusic); // считываем из памяти флаг перемешивания песенок
  if ( mixMusic > 1) mixMusic = 1;

  address = 120;
  EEPROM.get(address, barMan); // считываем из памяти флаг режима бармен
  if ( barMan > 1) barMan = 1;

  Procent = 0;

  delay (100);//Между двумя командами необходимо делать задержку 100 миллисекунд, в противном случае некоторые команды могут работать не стабильно.
  myMP3.EQSelect(EQ_NORMAL);
  delay (100);
  tracks = myMP3.numTracksInFolder(folder); // считываем колличество треков в папке 01-09 в корне флешки, не больше 100
  if (tracks > 100) tracks = 100;
  delay (100);
  myMP3.volume(volume);
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
