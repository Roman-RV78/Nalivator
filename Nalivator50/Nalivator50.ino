// ===========КОММЕРЧЕСКОЕ ИСПОЛЬЗОВАНИЕ В ЛЮБОМ ВИДЕ ЗАПРЕЩЕНО!!!===============

// ======== НАСТРОЙКИ ========
#define NUM_SHOTS 6       // количество рюмок (оно же кол-во светодиодов и концевиков!), всё делалось под 6 рюмок!!!!
#define DEBUG_UART 0  // отладка, 0 выключено, 1 включено
#define PRE_PAUSE 1000UL       // пауза серво перед движением к рюмке
#define POST_PAUSE 500UL  // пауза после остановки сервы до включения помпы
#define MAX_DRINK 50  // максимальный объём тары мл, для корректного отображения в менюшках экрана, ставить не более 95 и кратный 5
#define MIN_DRINK 20  // минимальный объём тары мл,  кратный 5
#define MEMORY_ON // включение запись параметров в память, закомментировано - значит выключено! 
#define SWITCH_LEVEL 1    // концевики 1 - высокий сигнал при замыкании, 0 - низкий
#define PUMP_LEVEL 1    // уровень включения помпы, 1 - высокий уровень, 0 - низкий
#define SOUND_THEME 0    // звуковая тема, 0 - по умолчанию, 1 - для танкистов
//#define STARTING_POS_SERVO_GLASS1 // раскомментировать, если нужна начальная позиция серво в первой рюмке
//#define SERVO_CHANGE_DIRECTION // раскомментировать, отзеркалить движение серво
//#define LED_CHANGE_DIRECTION // раскомментировать, отзеркалить позиции led диодов
//#define PIN_CHANGE_DIRECTION // раскомментировать, отзеркалить пины подключения концевиков
  #ifdef SERVO_CHANGE_DIRECTION 
     #define INITAL_ANGLE_SERVO 180 // начальный угол на который становится серво, если включен режим зеркало, подбирать в меньшую сторону, если упирается серво.
  #endif
#define BAT_MONITOR_ON // включение в коде всё что звязано с АКБ, закомментировать если нет АКБ вообще.
  #ifdef BAT_MONITOR_ON
     #define ADC_U_COEFF 0.00472 // коэффициент перевода кода АЦП в напряжение.  1.1 / 1023 / R4 * (R4 + R5).  Условие должно выполняться - R4 / ( R4 + R5 ) = 0.22 
     #define LOW_BAT_SLEEP_ON  // контроль низкого заряда акб. Закомментировать - выключить для отладки кода, если не подключен пин A7 к мониторингу АКБ, иначе сон
  #endif
//#define LED_TOWER  // светодиоды на башне. Раcкомментировать если есть
  #if (DEBUG_UART == 0)
    //#define BUTTON_TOWER  // сенсорная кнопка на башне. Раскомментировать если есть
  #endif
  
// изменить значения если серво не доезжает до нужных углов, не всегда эффективно!
#define SERVO_MIN 544 // уменьшить если не доезжает до 0° (544 по умолчанию)
#define SERVO_MAX 2400 // увеличить если не доезжает до 180° (2400 по умолчанию )

// цвет статусов налитых-пустых установленных рюмок, например здесь https://basicweb.ru/html/html_colors.php можно посмотреть и выставить понравившиеся
#define COLOR_POURET_GLASS 0 , 255 , 0 // цвет налитой рюмки R G B , здесь зелёный
#define COLOR_EMPTY_GLASS 255 , 0 , 0  // цвет пустой рюмки  R G B , здесь красный

// настройки для иллюминации стола после разлива и иллюминации наливающейся рюмки
#define INTENSITY_COLOR_CHANGE_GLASS 30 // можно поменять , интенсивность смены цвета при розливе на рюмке
#define TABLE_RAINBOW_STEP 60 //увеличивает шаг радуги стола после розлива, можно поиграться значением
#define NUMBER_ILLUMINATION_PASSES 24 // условное число проходов иллюминации стола после налива

// =========== ЛИБЫ ===========
#include <EEPROM.h>
#include <LCD_1602_RUS.h>
#include <DFPlayerMini_Fast.h>
#include "encUniversalMinim.h"
#include <Adafruit_TiCoServo.h>
#include "timer2Minim.h"
#include "menu2.h"
#include "random.h"

#define ORDER_GRB       // порядок цветов ORDER_GRB / ORDER_RGB / ORDER_BRG
#include <microLED.h>



//                 =========== ПИНЫ ARDUINO NANO ===========
#if  defined(__AVR_ATmega328P__)
  #include <SoftwareSerial.h> // подключаем плеер  не к железному сериал, обязательно!
  SoftwareSerial mySerial(10, 11); // RX, TX   плейер подключаем к D10 и  D11
#ifndef PIN_CHANGE_DIRECTION
   #if (NUM_SHOTS == 6)
     const uint8_t SW_pins[] = {A0, A1, A2, A3, 7, 8};  //  пины концевиков для arduino nano, 6 стопок
   #elif (NUM_SHOTS == 5)
     const uint8_t SW_pins[] = {A0, A1, A2, A3, 7};  //  пины концевиков для arduino nano, 5 стопок
   #elif (NUM_SHOTS == 4)
     const uint8_t SW_pins[] = {A0, A1, A2, A3};  //  пины концевиков для arduino nano, 4 стопки
   #else
    #error "ВЫБРАНО МАЛОЕ КОЛИЧЕСТВО РЮМОК!!!"  
   #endif
#else
  #if (NUM_SHOTS == 6)
   const uint8_t SW_pins[] = {8, 7, A3, A2, A1, A0};  //  пины концевиков для arduino nano, зеркально, 6 стопок
  #elif (NUM_SHOTS == 5)
   const uint8_t SW_pins[] = {7, A3, A2, A1, A0};  //  пины концевиков для arduino nano, зеркально, 5 стопок
  #elif (NUM_SHOTS == 4)
   const uint8_t SW_pins[] = {A3, A2, A1, A0};  //  пины концевиков для arduino nano, зеркально, 4 стопки
  #else
    #error "ВЫБРАНО МАЛОЕ КОЛИЧЕСТВО РЮМОК!!!"
  #endif
#endif 
  #define SERVO_PIN   9 // пин серво для arduino nano. Можно только 9, 10 пины!!!!!!!!
  #define PUMP_POWER 13 // помпа для arduino nano
  #define LED_PIN 6  // пин ленты для arduino nano
 #ifdef BAT_MONITOR_ON
   #define BAT_PIN A7 // пин замера напряжения акб для arduino nano
 #endif
  #define BUSY_PIN 12  // пин готовности DF плеера для arduino nano
 // ПИНЫ ЭНКОДЕРА
  #define CLK  2 //для arduino nano
  #define DT  3 // для arduino nano
  #define SW  4  // кнопка энкодера для arduino nano
 #ifdef LED_TOWER
    #define LED_PIN2 5  // пин второй ленты для arduino nano
 #endif
 #ifdef BUTTON_TOWER
    #define BUT_TOWER_PIN 1 // пин кнопки на башне для arduino nano
 #endif
//               =========== ПИНЫ ARDUINO MEGA ===========
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define SERVO_PIN   2 // пин серво для arduino mega. Можно только 2, 3, 5, 6, 7, 8, 11, 12, 13, 44, 45, 46 пины!!!!!!!!
#ifndef PIN_CHANGE_DIRECTION
  #if (NUM_SHOTS == 6)
   const uint8_t SW_pins[] = {A0, A1, A2, A3, A7, A8};  //  пины концевиков для arduino mega, 6 стопок
  #elif (NUM_SHOTS == 5)
   const uint8_t SW_pins[] = {A0, A1, A2, A3, A7};  //  пины концевиков для arduino mega, 5 стопок
  #elif (NUM_SHOTS == 4)
  const uint8_t SW_pins[] = {A0, A1, A2, A3};  //  пины концевиков для arduino mega, 4 стопки
  #else
    #error "ВЫБРАНО МАЛОЕ КОЛИЧЕСТВО РЮМОК!!!" 
  #endif
#else
  #if (NUM_SHOTS == 6)
  const uint8_t SW_pins[] = {A8, A7, A3, A2, A1, A0};  //  пины концевиков для arduino mega, зеркально, , 6 стопок
  #elif (NUM_SHOTS == 5)
  const uint8_t SW_pins[] = {A7, A3, A2, A1, A0};  //  пины концевиков для arduino mega, зеркально, 5 стопок
  #elif (NUM_SHOTS == 4)
  const uint8_t SW_pins[] = {A3, A2, A1, A0};  //  пины концевиков для arduino mega, зеркально, 4 стопки
  #else
    #error "ВЫБРАНО МАЛОЕ КОЛИЧЕСТВО РЮМОК!!!"
  #endif
#endif
  #define PUMP_POWER 12 // помпа для arduino mega
  #define LED_PIN 5  // пин ленты для arduino mega
 #ifdef BAT_MONITOR_ON
   #define BAT_PIN A9 // пин замера напряжения акб для arduino mega
 #endif
  #define BUSY_PIN 10  // пин готовности DF плеера для arduino mega
  // ПИНЫ ЭНКОДЕРА
  #define CLK  7 // для arduino mega
  #define DT  6// для arduino mega
  #define SW  4  // кнопка энкодера для arduino mega
  #ifdef LED_TOWER
    #define LED_PIN2 11  // пин второй ленты для arduino mega
  #endif
  #ifdef BUTTON_TOWER
    #define BUT_TOWER_PIN 22 // пин кнопки на башне для arduino mega
  #endif
#else
 #error "ВЫБРАНА НЕ ВЕРНАЯ ПЛАТА АРДУИНО!!!"
#endif






//               =========== ДАТА ===========
LEDdata leds[NUM_SHOTS];  // буфер ленты типа LEDdata (размер зависит от COLOR_DEBTH)
microLED strip(leds, NUM_SHOTS, LED_PIN);  // объект лента
#ifdef LED_TOWER
  #define NUMLEDS 14 // колличество светиков во второй ленте
  LEDdata leds2[NUMLEDS];  // буфер ленты типа LEDdata (размер зависит от COLOR_DEBTH)
  microLED strip2(leds2, NUMLEDS, LED_PIN2);  // объект лента
  timerMinim TOWERtimer(20);
  bool mig = false;
  bool rainbow = true;
  bool clearLed = false;
#endif
#ifdef BUTTON_TOWER
    bool stateBut = false;
#endif
encMinim enc(CLK, DT, SW, 0); // пин clk, пин dt, пин sw, направление (0/1)
LCD_1602_RUS lcd(0x27, 16, 2); //Адрес дисплея обычно 0x27 или 0x3F, подключение экрана А4-SDA-зеленый, А5-SCL-желтый
DFPlayerMini_Fast myMP3;
Adafruit_TiCoServo servo;



timerMinim LEDtimer(50);
timerMinim FLOWdebounce(20);
timerMinim WAITtimer(PRE_PAUSE);
timerMinim TIMEProcent(2000);
timerMinim SAVEtimer(30000); // таймер спящего режима
timerMinim PAUSEtimer(4000); // таймер паузы
timerMinim PLAYtimer(2000); // таймер переключения треков

int16_t time50ml;  // время налива 50 мл
uint8_t shotPos[] = {0, 0, 0, 0, 0, 0}; // позиция рюмок, в сетапе считается из памяти.
int16_t address = 0; // Переменная для хранения адреса памяти
int16_t bright = 0;  // яркость led, считается из памяти
int16_t Procent = 0;
int8_t count = -1;
int8_t curPumping = -1;
int16_t servoPos = 0;
int8_t volume;
int8_t volume2;
uint8_t subNastr = 1;
uint8_t subMush = 1;
uint8_t subMenu = 1;
uint8_t yesGlass = 0;
uint8_t randomGlass = 0;
uint8_t speedSer = 10;
int8_t tracks = 0;
int16_t tracks2 = 0;
uint8_t  Menu = 0;
uint8_t MenuFlag = 0; // Здесь храниться уровень меню.
uint8_t  Drink = MIN_DRINK; // 
uint8_t  DrinkCount = 0; //счётчик налитых рюмок
uint8_t folder;
uint8_t oldFolder;
uint8_t folder2 = 10;
uint8_t num = 0;
uint8_t num2 = 0;
int8_t sleepTime = 0;
uint8_t ManDrink[] = {0, 0, 0, 0, 0, 0};
uint8_t TostList[100]; // массив номеров треков тостов, в нём перемешаем всё, максимум 100 тостов. нужно ли столько?
uint8_t MusicList[100]; // массив номеров песенок, в нём перемешаем всё, максимум 100
uint8_t ManRum = 1;
uint8_t mixTracks = 0;
uint8_t mixMusic = 0;
uint8_t folTra = 1; //
uint8_t oldNum = 1; //
uint8_t barMan;
bool LEDchanged = false;
bool pumping = false;
bool promivka = false;
bool ledShow = true;
bool moving = false;
bool tost = false;
bool returnMenu = false;
bool check = false;
bool save = false;
bool systemON = false;
bool noGlass = true;
bool readyDrink = false; // стопки налиты
bool flag = false; // флаг что нужно показать меню налива
bool player = false; //
bool nextTrack = false;
bool playOn = false; // флаг что плеер был включен
bool ledShowOn = true;
bool pause = false;
bool noTost = false;
bool playMush = false;
bool flagMush = false;
uint8_t noDoliv;
uint8_t noTostBarmen;
int8_t rumka[NUM_SHOTS]; // массив рандомных позиций рюмок
bool readySystem = true;
int8_t drift = 0; 
#if(SOUND_THEME == 1)
  bool muveBack = true;
#endif

enum {NO_GLASS, EMPTY, IN_PROCESS, READY} shotStates[NUM_SHOTS];
enum {SEARCH, MOVING, WAIT, PUMPING} systemState;
// свои символы
#ifdef BAT_MONITOR_ON
int16_t value = 750;   // значение с аналогового пина подключенного к батарее
bool lowBat = false; // флаг низкого заряда акб
uint8_t battery0[8] = {B01110, B11111, B10001, B10001, B10001, B10001, B10001, B11111}; // пустая батарея
uint8_t battery1[8] = {B01110, B11111, B10001, B10001, B10001, B10001, B11111, B11111}; //  батарея одно деление
uint8_t battery2[8] = {B01110, B11111, B10001, B10001, B10001, B11111, B11111, B11111}; //  батарея два деления
uint8_t battery3[8] = {B01110, B11111, B10001, B10001, B11111, B11111, B11111, B11111}; //  батарея три деления
uint8_t battery4[8] = {B01110, B11111, B10001, B11111, B11111, B11111, B11111, B11111}; //  батарея четыре деления
uint8_t battery5[8] = {B01110, B11111, B11111, B11111, B11111, B11111, B11111, B11111}; //  батарея полная
uint8_t power[8] = {B01010, B01010, B11111, B11111, B11111, B01110, B00100, B00100}; // питание от сети
#endif


// =========== МАКРО ===========
#if (PUMP_LEVEL == 0)
  #define pumpON() digitalWrite(PUMP_POWER, 0)
  #define pumpOFF() digitalWrite(PUMP_POWER, 1)
#else
   #define pumpON() digitalWrite(PUMP_POWER, 1)
   #define pumpOFF() digitalWrite(PUMP_POWER, 0)
#endif

#if (DEBUG_UART == 1)
#define DEBUG(x) Serial.println(x)
#else
#define DEBUG(x)
#endif
