/*
  Работа arduino uno с модулем микрофона KY-037
  Измерение предельного значения шума в дБ
*/

#include <LiquidCrystal_I2C.h>
//  Объявляем  объект библиотеки, указывая параметры дисплея
// (адрес I2C = 0x27, количество столбцов = 16, количество строк = 2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pin_sensor = A0; // вывод arduino к которому подключен микрофон
const int LIMIT_NOISE = 80; // предельное значение шума в дБ

float filt(int input) {
  static float out = 0;
  out = out + 0.07f * (input - out);
  return out;
}

void setup() {
  lcd.init();                       //  Инициируем работу с LCD дисплеем
  lcd.backlight();                  //  Включаем подсветку LCD дисплея
  lcd.setCursor(0, 0);              //  Устанавливаем курсор в позицию (0 столбец, 0 строка)
  lcd.print("LEVEL: ");             //  Выводим текст
  lcd.setCursor(0, 1);
  lcd.print("NOISE IS NORMAL ");
}

void loop() {
  static unsigned long capture_time = millis(); // захват времени
  static int result = 0; // результат дБ после фильтрации
  static bool flag = 0; // флаг для однократной печати на дисплее

  int dB = 20 * log(analogRead(pin_sensor) / 5.0f); // вычисление дБ через напряжение
  if (millis() - capture_time >= 10) { // цикл равный 10мс
    result = filt(dB); // фильтруем сигнал, чтоб показания не прыгали
    if (result < 100) { // стираем символ после 99 т.к дисплей запоминает все символы
      lcd.setCursor(9, 0);
      lcd.print(' ');
    }
    lcd.setCursor(7, 0); // ставим курсор на нужное положение
    lcd.print(result); // печатаем дБ
    if (result >= LIMIT_NOISE && !flag) { // шум выше допустимого
      lcd.setCursor(0, 1); // ставим курсор
      lcd.print("NOISE > LIMIT   ");
      flag = !flag; // флаг для однократной печати на дисплее
    }
    else if (result < LIMIT_NOISE && flag) { // если значение меньше предельного шума
      lcd.setCursor(0, 1); // ставим курсор
      lcd.print("NOISE IS NORMAL ");
      flag = !flag; // флаг для однократной печати на дисплее
    }
    capture_time = millis(); // захват значения времени
  }

}
