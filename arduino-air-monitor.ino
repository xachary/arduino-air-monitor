#include <MemoryFree.h>

#include "display.h"
#include "clock.h"
#include "buttons.h"

#include "m_Temperature_Tumidity.h"
#include "m_UV.h"
#include "m_TVOC.h"
#include "m_CO2.h"
#include "m_HCHO.h"

// HCHO：
// 80ug/m3（即国家室内标准0.08mg/m3）

// TVOC(指数)：
// 优秀[0~100]、越大越差[100~500]

// CO2(ppm)：
// 室外[350～450]、清新[350～1000]、浑浊[1000～2000]、头痛[2000～5000]、缺氧[‌>5000]

// UV(指数)：
// 白0[<50]、绿1[227]、绿2[318]、黄3[408]、黄4[503]、黄5[606]、橙6[696]、橙7[795]、红8[881]、红9[976]、红10[1079]、紫11[1170+]

// Last process time
long lastProcessSecond = 0;

void setup() {
  Serial.begin(115200);

  Buttons::Btn_1.init();

  Display::OLED.init();
  Clock::RTC.init();

  Module::TVOC.init();
  Module::HCHO.init();
  Module::CO2.init();
}

void process(bool display) {
  // 设置当前环境中的相对湿度和温度，获得更精确的数值
  if (Module::TVOC.isWarmUp) {
    Module::TVOC.adjust(Module::Temperature.getValue(), Module::Humidity.getValue());
  }

  Module::HCHO.read();

  // char nowStr[20] = "";
  // now.tostr(nowStr);
  // Serial.println(nowStr);
  // Serial.print("Temperature:");
  // Serial.print(Module::Temperature.getValue());
  // Serial.println(Module::Temperature.unit);
  // Serial.print("Humidity:");
  // Serial.print(Module::Humidity.getValue());
  // Serial.println(Module::Humidity.unit);
  // Serial.print("UV:");
  // Serial.print(Module::UV.getValue());
  // Serial.println(Module::UV.unit);
  // Serial.print("CO2:");
  // Serial.print(Module::CO2.getValue());
  // Serial.println(Module::CO2.unit);
  // Serial.print("TVOC:");
  // Serial.print(Module::TVOC.getValue());
  // Serial.println(Module::TVOC.unit);
  // Serial.print("HCHO:");
  // Serial.print(Module::HCHO.getValue());
  // Serial.println(Module::HCHO.unit);

  if (display) {
    Display::OLED.clearBlockCenter(printHCHO_UGM3(Module::HCHO.getValue(), 3, false), printUV(Module::UV.getValue(), 3, true), 3);
    Display::OLED.clearBlockCenter(printTVOC(Module::TVOC.getValue(), 5, false), printTemp(Module::Temperature.getValue(), 5, true), 5);
    Display::OLED.clearBlockCenter(printCO2(Module::CO2.getValue(), 7, false), printHum(Module::Humidity.getValue(), 7, true), 7);
  }
}

bool oledDisplay = true;

void loop() {
  Buttons::Btn_1.loop();

  bool clicked = Buttons::Btn_1.getValue();

  if (clicked) {
    oledDisplay = !oledDisplay;
  }

  if (oledDisplay) {
    Display::OLED.clearBlockCenter(printDate(0, false), SSD_1306::width * 0.5, 0);
    Display::OLED.clearBlockCenter(printTime(1, false), SSD_1306::width * 0.5, 1);
    printTitle();
  } else {
    Display::OLED.clearScreen();
  }

  if (!Module::TVOC.isWarmUp) {
    Module::TVOC.warmUp();
  }

  // process each second
  if (millis() / 1000 - lastProcessSecond >= 1) {
    Serial.println();

    Serial.print("loop:");
    Serial.print(millis() / 1000);
    Serial.println("s");

    process(oledDisplay);

    lastProcessSecond = millis() / 1000;
  }
}

void printTitle() {
  char str[SSD_1306::charMax] = "[Air]";

  unsigned int w = Display::OLED.getTextSize(str) * 2;

  Display::OLED.printNRaw(SSD_1306::width - w, 0, str);
}

unsigned int printMem() {
  char str[SSD_1306::charMax] = "";

  strcat(str, "Memory");

  unsigned int w = Display::OLED.getTextSize(str);

  Display::OLED.printRaw(SSD_1306::width - w, SSD_1306::lineHeight * 0, str);

  return w;
}

unsigned int printMemPercent() {
  char str[SSD_1306::charMax] = "";

  char numStr[SSD_1306::charMax] = "";
  dtostrf(freeMemory(), 0, 0, numStr);

  strcat(str, numStr);
  strcat(str, "bytes");

  unsigned int w = Display::OLED.getTextSize(str);

  Display::OLED.printRaw(SSD_1306::width - w, SSD_1306::lineHeight * 1, str);

  return w;
}

unsigned int printDate(unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";

  char date[SSD_1306::charMax] = "";

  DateTime now = Clock::RTC.now();
  now.tostr(date);

  strncpy(str, date, 10);

  if (isRight) {
    Display::OLED.printRight(str, row);
  } else {
    Display::OLED.print(str, 0, row);
  }

  // Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printTime(unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";

  char time[SSD_1306::charMax] = "";

  DateTime now = Clock::RTC.now();
  now.tostr(time);

  strncpy(str, time + 11, 8);

  if (isRight) {
    Display::OLED.printRight(str, row);
  } else {
    Display::OLED.print(str, 0, row);
  }

  // Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printHCHO_UGM3(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "HCHO:");

  unsigned int num = value;
  if (num > 1000) {
    num = 0;
  }
  char numStr[SSD_1306::charMax] = "";
  itoa(num, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  unsigned int w = Display::OLED.getTextSize(str) + 1;

  return Display::OLED.getTextSize(str) + printPower3(w, row);
}

unsigned int printTVOC(float value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "TVOC:");

  char numStr[SSD_1306::charMax] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);

  if (isRight) {
    Display::OLED.printRight(str, row);
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}
unsigned int printTemp(float value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "Temp:");

  char numStr[SSD_1306::charMax] = "";
  dtostrf(value, 1, 1, numStr);
  strcat(str, numStr);

  strcat(str, " C");

  unsigned int w = Display::OLED.getTextSize("C") + 1;

  if (isRight) {
    Display::OLED.printRight(str, row);

    printDeg(SSD_1306::width - w - 3, row);
  } else {
    Display::OLED.print(str, 0, row);

    printDeg(w - 3, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printHum(float value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "Hum:");

  char numStr[SSD_1306::charMax] = "";
  dtostrf(value, 1, 1, numStr);
  strcat(str, numStr);
  strcat(str, "%");

  if (isRight) {
    Display::OLED.printRight(str, row);
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printCO2(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "CO2:");

  char numStr[SSD_1306::charMax] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ppm");

  if (isRight) {
    Display::OLED.printRight(str, row);
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printUV(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "UV:");

  char numStr[SSD_1306::charMax] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);

  if (isRight) {
    Display::OLED.printRight(str, row);
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  return Display::OLED.getTextSize(str);
}

unsigned int printPM1(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "PM1:");

  char numStr[SSD_1306::charMax] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  if (isRight) {
    Display::OLED.printRight(str, row);
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  unsigned int w = Display::OLED.getTextSize(str) + 1;

  return Display::OLED.getTextSize(str) + printPower3(w, row);
}

unsigned int printPM2_5(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "PM2.5:");

  char numStr[SSD_1306::charMax] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  if (isRight) {
    Display::OLED.printRight(str, row);
    ;
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  unsigned int w = Display::OLED.getTextSize(str) + 1;

  return Display::OLED.getTextSize(str) + printPower3(w, row);
}

unsigned int printPM10(unsigned int value, unsigned int row, bool isRight) {
  char str[SSD_1306::charMax] = "";
  strcat(str, "PM10:");

  char numStr[SSD_1306::charMax] = "";
  itoa(value, numStr, 10);
  strcat(str, numStr);
  strcat(str, "ug/m");

  if (isRight) {
    Display::OLED.printRight(str, row);
  } else {
    Display::OLED.print(str, 0, row);
  }

  Serial.println(str);

  unsigned int w = Display::OLED.getTextSize(str) + 1;

  return Display::OLED.getTextSize(str) + printPower3(w, row);
}

unsigned int printPower3(unsigned int left, unsigned int top) {
  // 绘制立方"³"符号
  // 位图方向：从左往右、从下往上
  // 0 0 0
  // 0 0 0
  // 0 0 0
  // 1 1 1
  // 0 0 1
  // 1 1 1
  // 0 0 1
  // 1 1 1
  // 第一列 00010101 -> 0x15
  // 第二列 00010101 -> 0x15
  // 第三列 00011111 -> 0x1F
  // js转换示例：parseInt('00011111',2) -> (31).toString(16) -> 1f
  uint8_t buffer[3] = { 0x15, 0x15, 0x1F };
  Display::OLED.drawBuffer(left, top, buffer);

  return 4;
}

unsigned int printDeg(unsigned int left, unsigned int top) {
  // 绘制"°"符号
  // 位图方向：从左往右、从下往上
  // 0 0 0
  // 0 0 0
  // 0 0 0
  // 0 0 0
  // 0 0 0
  // 0 1 0
  // 1 0 1
  // 0 1 0
  uint8_t buffer[3] = { 0x02, 0x05, 0x02 };
  Display::OLED.drawBuffer(left, top, buffer);

  return 4;
}
