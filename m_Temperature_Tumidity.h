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
    if (value > 50 || value < -50) {
      value = lastValue;
    }
    lastValue = value;
    return value;
  }

  char unit[3] = " C";
} Temperature;

struct _Humidity {
  float lastValue = 70;
  float getValue() {
    float value = _m_dht_22.getHumidity();
    if (value > 100 || value < 0) {
      value = lastValue;
    }
    lastValue = value;
    return value;
  }

  char unit[2] = "%";
} Humidity;

}