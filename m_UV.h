#include <Arduino.h>

// 来源于网络示例
// https://www.sohu.com/a/694634336_120248280


// 紫外线指数
// 型号：guva_s12sd
// 接口：VCC->VCC(5V)、GND->GND、SIO->A7
// 协议：ADC

#define _Pin_UV A7

namespace Module {

struct _UV {
  int getValue() {
    // float each = (float)5 / (float)1024;
    // float v = each * (float)analogRead(_Pin_UV);
    // return v * 10;

    float value = analogRead(_Pin_UV);
    return value / (float)1024 * 3.3;
  }

  char unit[6] = "index";
} UV;

}