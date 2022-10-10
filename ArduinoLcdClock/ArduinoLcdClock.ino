#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DS3232RTC.h>
#include <LiquidCrystal_I2C.h>

Adafruit_BME280 bme280;

DS3232RTC ds3232RTC;
int lastYear = -1;
int lastMonth = -1;
int lastDay = -1;
int lastHour = -1;
int lastMinute = -1;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  bme280.begin(0x76);

  ds3232RTC.begin();
  setTime(23, 20, 0, 10, 10, 2022);
  ds3232RTC.set(now());

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(15, 0);
  lcd.print("C");
  lcd.setCursor(15, 1);
  lcd.print("%");
  lcd.setCursor(4, 0);
  lcd.print(":");
  lcd.setCursor(7, 0);
  lcd.print(":");
  lcd.setCursor(4, 1);
  lcd.print("/");
  lcd.setCursor(7, 1);
  lcd.print("/");
}

void loop() {
  updateBME280();
  updateDS3232RTC();
  delay(1000);
}

void updateBME280() {
  char tempString[5];
  char humString[5];

  dtostrf(bme280.readTemperature(), 4, 1, tempString);
  dtostrf(bme280.readHumidity(), 4, 1, humString);

  lcd.setCursor(11, 0);
  lcd.print(tempString);
  lcd.setCursor(11, 1);
  lcd.print(humString);
}

void updateDS3232RTC() {
  tmElements_t tm;
  char buffer[5];

  ds3232RTC.read(tm);

  sprintf(buffer, "%02d", tm.Second);
  lcd.setCursor(8, 0);
  lcd.print(buffer);

  if (tm.Minute != lastMinute) {
    sprintf(buffer, "%02d", tm.Minute);
    lcd.setCursor(5, 0);
    lcd.print(buffer);
    lastMinute = tm.Minute;
  }

  if (tm.Hour != lastHour) {
    sprintf(buffer, "%02d", tm.Hour);
    lcd.setCursor(2, 0);
    lcd.print(buffer);
    lastHour = tm.Hour;
  }

  if (tm.Day != lastDay) {
    sprintf(buffer, "%02d", tm.Day);
    lcd.setCursor(8, 1);
    lcd.print(buffer);
    lastDay = tm.Day;
  }

  if (tm.Month != lastMonth) {
    sprintf(buffer, "%02d", tm.Month);
    lcd.setCursor(5, 1);
    lcd.print(buffer);
    lastMonth = tm.Month;
  }

  if (tm.Year != lastYear) {
    sprintf(buffer, "%04d", tm.Year + 1970);
    lcd.setCursor(0, 1);
    lcd.print(buffer);
    lastYear = tm.Year;
  }
}