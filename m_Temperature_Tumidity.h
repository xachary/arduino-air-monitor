#include <Arduino.h>

#include <DHT22.h>

// 温湿度
// 型号：dht_22
// 接口：DAT->A1(当作D用)、VCC->VCC(5V)、GND->GND
// 协议：1-Wire(单总线)

#define _Pin_Temperature_Humidity A1

namespace Module {

DHT22 _m_dht_22(_Pin_Temperature_Humidity);

struct _Temperature {
  float lastValue = 25;

  float getValue() {
    float value = _m_dht_22.getTemperature();

    // 容错交给网页处理，这里只保证在量程之内
    if (value < -50 || value > 50) {
      value = lastValue;
    } else {
      lastValue = value;
    }
    return value;
  }

  char unit[3] = " C";
} Temperature;

struct _Humidity {
  float lastValue = 70;

  float getValue() {
    float value = _m_dht_22.getHumidity();

    // 容错交给网页处理，这里只保证在量程之内
    if (value < 0 || value > 100) {
      value = lastValue;
    } else {
      lastValue = value;
    }
    return value;
  }

  char unit[2] = "%";
} Humidity;

}