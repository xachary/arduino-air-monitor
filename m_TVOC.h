#include <Arduino.h>

#include <SoftI2C.h>

#include "DFRobot_SGP40.h"

// TVOC指数
// 型号：ccs_811
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
    int timeout = 0;
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

  int getValue() {
    return isWarmUp ? _m_sgp_40.getVoclndex() : 0;
  }

  char unit[6] = "index";
} TVOC;

}