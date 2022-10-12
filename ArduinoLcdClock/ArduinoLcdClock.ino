#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DS3232RTC.h>
#include <LiquidCrystal_I2C.h>

// センサーインスタンス
Adafruit_BME280 bme280;

// RTCインスタンス
DS3232RTC rtc;

// LCDインスタンス
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 時刻設定モードカウンタ
int setRTCCount = 10;

// 直前の日時表示値
int lastYear = -1;
int lastMonth = -1;
int lastDay = -1;
int lastHour = -1;
int lastMinute = -1;

void setup() {
  // センサー初期化
  bme280.begin(0x76);

  // RTC初期化
  rtc.begin();
  setSyncProvider(rtc.get);

  // LCD初期化
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

  // シリアル通信開始
  Serial.begin(115200);
}

void loop() {
  if (0 < setRTCCount) {
    if (0 < Serial.available()) {
      // シリアルモニタからメッセージを受信した場合は時刻設定モードに入る
      setRTC();
      setRTCCount = 0;
    } else {
      setRTCCount--;
    }
  }

  // 時刻設定モードカウンタが0になった場合はシリアル通信を終了する
  if (0 == setRTCCount) {
    Serial.end();
    setRTCCount = -1;
  }

  // センサー値と時計の更新
  updateBME280();
  updateRTC();

  delay(1000);
}

void setRTC() {
  tmElements_t tm;
  time_t tt;

  // シリアルバッファを空にする
  while (0 < Serial.available()) {
    Serial.read();
  }

  Serial.println("setRTC: yyyy,m,d,h,m,s");

  // シリアルモニターから設定日時が入力されるまで待つ
  while (Serial.available() < 14) {
    delay(100);
  }

  // 日時を設定する
  tm.Year = CalendarYrToTm(Serial.parseInt());
  tm.Month = Serial.parseInt();
  tm.Day = Serial.parseInt();
  tm.Hour = Serial.parseInt();
  tm.Minute = Serial.parseInt();
  tm.Second = Serial.parseInt();
  tt = makeTime(tm);
  rtc.set(tt);
  setTime(tt);

  Serial.print("Date:");
  Serial.print(year());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.println(day());
  Serial.print("Time:");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.println(second());
}

void updateBME280() {
  char buffer[5];

  dtostrf(bme280.readTemperature(), 4, 1, buffer);
  lcd.setCursor(11, 0);
  lcd.print(buffer);

  dtostrf(bme280.readHumidity(), 4, 1, buffer);
  lcd.setCursor(11, 1);
  lcd.print(buffer);
}

void updateRTC() {
  char buffer[5];
  int value;

  value = second();
  sprintf(buffer, "%02d", value);
  lcd.setCursor(8, 0);
  lcd.print(buffer);

  value = minute();
  if (value != lastMinute) {
    sprintf(buffer, "%02d", value);
    lcd.setCursor(5, 0);
    lcd.print(buffer);
    lastMinute = value;
  }

  value = hour();
  if (value != lastHour) {
    sprintf(buffer, "%02d", value);
    lcd.setCursor(2, 0);
    lcd.print(buffer);
    lastHour = value;
  }

  value = day();
  if (value != lastDay) {
    sprintf(buffer, "%02d", value);
    lcd.setCursor(8, 1);
    lcd.print(buffer);
    lastDay = value;
  }

  value = month();
  if (value != lastMonth) {
    sprintf(buffer, "%02d", value);
    lcd.setCursor(5, 1);
    lcd.print(buffer);
    lastMonth = value;
  }

  value = year();
  if (value != lastYear) {
    sprintf(buffer, "%04d", value);
    lcd.setCursor(0, 1);
    lcd.print(buffer);
    lastYear = value;
  }
}