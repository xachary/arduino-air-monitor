#include <Arduino.h>

// 来源于网络示例
// https://www.sohu.com/a/694634336_120248280
// https://wiki.dfrobot.com.cn/_SKU_SEN0162__UV_Sensor


// 紫外线指数
// 型号：guva_s12sd
// 接口：VCC->VCC(5V)、GND->GND、SIO->A7
// 协议：ADC

#define _Pin_UV A7

namespace Module {

struct _UV {
  float lastValue = 0;

  int getValue() {
    // 0~1023
    int analogValue = analogRead(_Pin_UV);
    // mV
    float voltageValue = (float)analogValue / 1024 * 5 * 1000;

    // 容错交给网页处理，这里只保证在量程之内
    if (voltageValue < 0 || voltageValue > 5000) {
      voltageValue = lastValue;
    } else {
      lastValue = voltageValue;
    }
    return voltageValue;
  }

  char unit[6] = "index";
} UV;

}