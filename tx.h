#include <SoftwareSerial.h>

// 实际可不接此引脚
#define _Pin_RX 10

// 实际只用此引脚
#define _Pin_TX 11

// 起始符
#define _Frame_Start 0XFF

// 数据帧长度：
// 起始符 x 2 [2字节]
// display   [1字节]
// hcho      [4字节]
// tvoc      [4字节]
// co2       [4字节]
// temp      [4字节]
// hum       [4字节]
// uv        [4字节]
// 检验位     [4字节]
#define _Frame_Len 31

namespace Transfer {

SoftwareSerial _serial(_Pin_RX, _Pin_TX);

// 测试各类型长度
// Serial.println(sizeof(uint8_t));  // 1
// Serial.println(sizeof(float));    // 4
// Serial.println(sizeof(int32_t));  // 4
// Serial.println();

uint8_t floatSize = sizeof(float);    // 4
uint8_t int32Size = sizeof(int32_t);  // 4

unsigned char floatBytes[4];
unsigned char int32Bytes[4];

struct _TX {
  void init() {
    _serial.begin(9600);
  }

  // display: 是否显示
  void send(uint8_t display, float hcho, float tvoc, float co2, float temp, float hum, float uv) {
    // 数据帧
    byte buffer[_Frame_Len] = {};

    // 下标
    uint8_t idx = 0;

    // 起始符
    buffer[0] = _Frame_Start;
    buffer[1] = _Frame_Start;

    idx += 2;

    // 是否显示
    buffer[2] = (unsigned char)display;

    idx += 1;

    // 4位/指标

    memcpy(floatBytes, &hcho, floatSize);
    for (uint8_t i = 0; i < floatSize; i++) {
      buffer[idx + i] = (unsigned char)floatBytes[i];
    }

    idx += floatSize;

    memcpy(floatBytes, &tvoc, floatSize);
    for (uint8_t i = 0; i < floatSize; i++) {
      buffer[idx + i] = (unsigned char)floatBytes[i];
    }

    idx += floatSize;

    memcpy(floatBytes, &co2, floatSize);
    for (uint8_t i = 0; i < floatSize; i++) {
      buffer[idx + i] = (unsigned char)floatBytes[i];
    }

    idx += floatSize;

    memcpy(floatBytes, &temp, floatSize);
    for (uint8_t i = 0; i < floatSize; i++) {
      buffer[idx + i] = (unsigned char)floatBytes[i];
    }

    idx += floatSize;

    memcpy(floatBytes, &hum, floatSize);
    for (uint8_t i = 0; i < floatSize; i++) {
      buffer[idx + i] = (unsigned char)floatBytes[i];
    }

    idx += floatSize;

    memcpy(floatBytes, &uv, floatSize);
    for (uint8_t i = 0; i < floatSize; i++) {
      buffer[idx + i] = (unsigned char)floatBytes[i];
    }

    idx += floatSize;

    // 检验位计算（指标位之和）
    int32_t sum = 0;
    for (int i = 2; i < _Frame_Len - int32Size; i++) {
      sum += buffer[i];
    }

    Serial.println();
    Serial.print("sum:");
    Serial.println(sum);

    memcpy(int32Bytes, &sum, int32Size);
    for (uint8_t i = 0; i < int32Size; i++) {
      buffer[idx + i] = (unsigned char)int32Bytes[i];
    }

    // 发送
    for (uint8_t i = 0; i < _Frame_Len; i++) {
      _serial.write(buffer[i]);
      Serial.print(buffer[i]);
      Serial.print(",");
    }
    Serial.println();
    Serial.println();
  }
} TX;

}