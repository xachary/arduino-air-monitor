#include <Arduino.h>

#include <SoftI2C.h>

#include "DFRobot_SGP40.h"

// TVOC指数
// 型号：sgp_40
// 接口：VCC->VCC(5V)、GND->GND、SDA->D6、SCL->D7、WAK->GND
// 协议：I2C(软)
// 地址：0x5A

#define _Pin_TVOC_SDA 6
#define _Pin_TVOC_SCL 7
#define _Address_TVOC 0x59

namespace Module {

SoftI2C SoftWire = SoftI2C(_Pin_TVOC_SDA, _Pin_TVOC_SCL);  //sda, scl
DFRobot_SGP40 _m_sgp_40(&SoftWire);

struct _TVOC {
  void init() {
    // 预热10秒
    _m_sgp_40.begin(10000);
  }

  bool isWarmUp = false;

  void warmUp() {
    if (!isWarmUp) {
      isWarmUp = _m_sgp_40.warmUp();
    }
  }

  void adjust(float temp, float hum) {
    _m_sgp_40.setRhT(hum, temp);
  }

  int lastValue = 0;

  int getValue() {
    int value = isWarmUp ? _m_sgp_40.getVoclndex() : 0;

    // 容错交给网页处理，这里只保证在量程之内
    if (value < 0 || value > 1000) {
      value = lastValue;
    } else {
      lastValue = value;
    }
    return value;
  }

  char unit[6] = "index";
} TVOC;

}