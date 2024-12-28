#include <Arduino.h>

// SD
#include <SPI.h>
#include <SD.h>

// SD
// SPI：MOSI->D11、MISO->D12、SCK->D13
// Pin：VCC->VCC(5V)、GND->GND、CS->D10
// Protocol：SPI

namespace Storage {

struct _SD_Card {
  bool init(int pin, char* error) {
    Serial.println("SD Init");

    if (!SD.begin(pin)) {
      strcpy(error, "SD failed.");
      Serial.println(error);
      return false;
    } else {
      return true;
    }
  }

  void getDate(unsigned int year, unsigned int month, unsigned int day, char date[10]) {
    char yearStr[5] = "";
    char monthStr[3] = "";
    char dayStr[3] = "";

    itoa(year, yearStr, 10);
    itoa(month, monthStr, 10);
    itoa(day, dayStr, 10);

    strcpy(date, "");

    strcat(date, yearStr);

    if (month < 10) {
      strcat(date, "0");
    }
    strcat(date, monthStr);

    if (day < 10) {
      strcat(date, "0");
    }
    strcat(date, dayStr);
  }

  void getTime(unsigned int hour, unsigned int minute, unsigned int second, char time[10]) {
    char hourStr[3] = "";
    char minuteStr[3] = "";
    char secondStr[3] = "";

    itoa(hour, hourStr, 10);
    itoa(minute, minuteStr, 10);
    itoa(second, secondStr, 10);

    strcpy(time, "");

    if (hour < 10) {
      strcat(time, "0");
    }
    strcat(time, hourStr);

    if (minute < 10) {
      strcat(time, "0");
    }
    strcat(time, minuteStr);

    if (second < 10) {
      strcat(time, "0");
    }
    strcat(time, secondStr);
  }

  void getFilename(char mode[], unsigned int year, unsigned int month, unsigned int day, char str[30]) {
    strcpy(str, mode);
    strcat(str, "\/");

    char date[10] = "";

    getDate(year, month, day, date);

    strcat(str, date);

    strcat(str, ".TXT");
  }

  void readLog24Hours(char mode[], unsigned int year, unsigned int month, unsigned int day, float values[24]) {
    char filename[30] = "";
    getFilename(mode, year, month, day, filename);

    Serial.print("ReadLog:");
    Serial.println(filename);

    float sums[24];
    unsigned int counts[24];
    char received[30] = "";
    unsigned int idx = 0;
    char ch;
    unsigned int split = 0;

    File dataFile = SD.open(filename);
    if (dataFile) {
      for (unsigned int i = 0; i < 24; i++) {
        sums[i] = 0;
        counts[i] = 0;
      }

      while (dataFile.available()) {
        ch = dataFile.read();
        if (ch == '\n' || ch == '\r') {
          if (ch == '\r') {
            dataFile.read();

            received[idx] = '\0';

            Serial.print("received:");
            Serial.println(received);

            int start = 0;
            int len = strlen(received);

            // Serial.print("len:");
            // Serial.println(len);

            // Serial.print("split:");
            // Serial.println(split);

            char hourStr[3] = "";
            hourStr[0] = received[0];
            hourStr[1] = received[1];
            hourStr[2] = '\0';

            // Serial.print("hourStr:");
            // Serial.println(hourStr);

            int hourNum = atoi(hourStr);

            // Serial.print("hourNum:");
            // Serial.println(hourNum);

            char valueStr[30] = "";
            unsigned int i = 0;
            for (; i < len - (split + 1); i++) {
              valueStr[i] = received[i + split + 1];
            }
            valueStr[len - (split + 1)] = '\0';

            // Serial.print("valueStr:");
            // Serial.println(valueStr);

            float value = atof(valueStr);

            // Serial.print("value:");
            // Serial.println(value);

            // 排除“无数据”、“异常数据”
            if (value != 0 && value > -100 && value < 5000) {
              sums[hourNum] += value;
              counts[hourNum]++;
            }

            idx = 0;
            split = 0;
          }
        } else {
          if (ch == '_') {
            split = idx;
          }

          received[idx] = ch;
          idx++;
        }
      }

      dataFile.close();

      Serial.println("ReadLog done");
    } else {
      Serial.println("ReadLog error");
    }

    for (unsigned int i = 0; i < 24; i++) {
      if (counts[i] > 0) {
        values[i] = sums[i] / (float)counts[i];
      }
    }
  }

  void writeLog(char mode[], unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second, float value) {
    if (!SD.exists(mode)) {
      SD.mkdir(mode);
    }

    char filename[30] = "";
    getFilename(mode, year, month, day, filename);

    Serial.print("writeLog:");
    Serial.println(filename);

    File dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
      char record[30] = "";
      getTime(hour, minute, second, record);

      strcat(record, "_");

      char valueStr[10] = "";
      // dtostrf(value, valueStr, 10);
      dtostrf(value, 1, 2, valueStr);
      // gcvt(value, valueStr);
      strcat(record, valueStr);

      dataFile.println(record);
      Serial.println(record);

      dataFile.close();

      Serial.println("writeLog done");
    } else {
      Serial.println("writeLog error");
    }
  }

  void delFile(char filename[30]) {
    Serial.print("delFile:");
    Serial.println(filename);

    if (SD.exists(filename)) {
      SD.remove(filename);
      Serial.print("delFile done");
    }
  }

  // 测试方法

  void _UnitTest() {
    Serial.print("SD_Card Test:");
    char date[10] = "";
    getDate(2024, 9, 1, date);
    Serial.println("date:");
    Serial.println(date);

    char time[10] = "";
    getTime(1, 2, 3, time);
    Serial.println("time:");
    Serial.println(time);

    char filename[30] = "";
    getFilename("HCHO", 2024, 8, 2, filename);
    Serial.println("filename:");
    Serial.println(filename);
  }

  void _DataTest(char mode[], unsigned int year, unsigned int month, unsigned int day) {
    if (!SD.exists(mode)) {
      SD.mkdir(mode);
    }

    char filename[30] = "";
    getFilename(mode, year, month, day, filename);

    Serial.print("DataTest:");
    Serial.println(filename);

    File dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
      char record[30] = "";
      char valueStr[10] = "";

      for (unsigned int hour = 0; hour < 24; hour++) {
        for (unsigned int minute = 0; minute < 60; minute++) {
          for (unsigned int second = 0; second < 1; second++) {
            getTime(hour, minute, second, record);
            strcat(record, "_");

            itoa(rand() % 10000, valueStr, 10);
            strcat(record, valueStr);

            dataFile.println(record);
            Serial.println(record);
          }
        }
      }

      dataFile.close();

      Serial.println("DataTest done");
    } else {
      Serial.println("DataTest error");
    }
  }
} SD_Card;

}