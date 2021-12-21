/* ================================================
    Контроллер для светодиодной ленты для стола


    by Ya0slav
    ================================================
*/


/*==========================================
              Настройки программы
  ========================================= */
#define SPEED 5 // Сколько добавлять к яркости за 1 сдвиг
#define ON_OFF_BUTTON_PIN 2 // Пин вкл/выкл кнопки(Лучше не менять)


/*==========================================
                   Энкодер
  ========================================= */
#define ENCODER_PIN_S1 3 // Пин S1 энкодера
#define ENCODER_PIN_S2 4 // Пин S2 энкодера


/*==========================================
                    Свет
  ========================================= */
#define LIGHT_PIN 9 // Пин для подключения ленты(через транзистор)
#define RED_PIN 5 // Нужен для состояния блокировки
#define GREEN_PIN 6 // Для состояния включения


/*==========================================
                   Дисплей
  ========================================= */
#define CLK 7 //Пин CLK
#define DIO 8 //Пин DIO
#define DISPLAY_TIMEOUT 3000 // Через сколько миллисекунд тухнет 
                             // дисплей и/или красный светодиод





 





/*==========================================
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

             ОСНОВНАЯ ПРОГРАММА

   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  ========================================= */
#include <EncButton.h>
#include <GyverTM1637.h>

uint32_t disp_tmr;
volatile uint32_t btn_filter;

int i;
volatile bool isEnable = true;

GyverTM1637 disp(CLK, DIO);
EncButton<EB_TICK, ENCODER_PIN_S1, ENCODER_PIN_S2> enc;

void setup() {

  //Разгон шим
  TCCR1A = 0b00000001;  
  TCCR1B = 0b00001001;

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(ON_OFF_BUTTON_PIN, INPUT_PULLUP);

  disp.clear();
  disp.brightness(7);

  attachInterrupt(digitalPinToInterrupt(ON_OFF_BUTTON_PIN), on_off, FALLING);
}

void loop() {

  encoder_logic();

  if (millis() - disp_tmr < DISPLAY_TIMEOUT) {
    disp.displayInt((int)map(i, 0, 255, 0, 100));
    digitalWrite(RED_PIN, !isEnable);
  }
  else {
    disp.clear();
    digitalWrite(RED_PIN, LOW);
  }

  digitalWrite(GREEN_PIN, isEnable);


  if (isEnable){
    analogWrite(LIGHT_PIN, i);
  }
  else analogWrite(LIGHT_PIN,LOW);
  
}

void encoder_logic() {
  enc.tick();

  if (!enc.turn()) return;
  disp_tmr = millis();
  if (!isEnable) return;


  if (enc.left()) {
    i -= SPEED;
    if (i < 0) i = 0;
  }
  else if (enc.right()) {
    i += SPEED;
    if (i > 255) i = 255;
  }

}

void on_off() {
  if ( millis() - btn_filter < 300) return; //Антидребезг
  
  isEnable = !isEnable;
  disp_tmr = millis();
  btn_filter = millis();
}
