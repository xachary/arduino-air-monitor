#include <Arduino.h>
#include <SoftwareSerial.h>

// 基于WZ Library（由于该库使用了Stream.h，出现无法解释的异常，因此简单重写之）

// 甲醛传感器
// 型号：WZ-S
// 接口：5V->VCC(5V)、G->GND、R->D9(RX)、T->D8(TX)
// 协议：UART

#define _Pin_HCHO_RX 8
#define _Pin_HCHO_TX 9
#define _Frame_Start_HCHO 0Xff  // 帧 起始符
#define _Frame_Len_HCHO 9       // 帧长度

namespace Module {

SoftwareSerial _serial_hcho(_Pin_HCHO_RX, _Pin_HCHO_TX);

namespace WZ_S {

// 帧数据
byte buffer[_Frame_Len_HCHO] = {};

// 帧数据组装完成
bool buffer_done = false;

// 上一次的值
int lastData = 0;
// 值
int data = 0;

void passiveMode() {
  // 0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46
  _serial_hcho.write((int)0xFF);
  _serial_hcho.write((int)0x01);
  _serial_hcho.write((int)0x78);
  _serial_hcho.write((int)0x41);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x46);
}

void requestRead() {
  // 0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79
  _serial_hcho.write((int)0xFF);
  _serial_hcho.write((int)0x01);
  _serial_hcho.write((int)0x86);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x00);
  _serial_hcho.write((int)0x79);
}

void Calc() {
  unsigned int timeout = 0;

  // 位序号
  int count = 0;

  // 起始
  bool flag_start = false;
  // 结束
  bool flag_end = false;

  // 当前位
  byte byte_val = 0x00;

  // 容错10次
  while (flag_end == false && timeout < _Frame_Len_HCHO * 10) {
    timeout++;

    // 串口可用
    if (_serial_hcho.available() > 0) {
      // 按位读数
      char int_val = (char)_serial_hcho.read();
      byte_val = (byte)int_val;

      // 帧数据组装
      buffer[count] = byte_val;

      // 起始符
      if (byte_val == _Frame_Start_HCHO) {
        // 更新 位序号
        count = 0;
        // 起始
        flag_start = true;
      }

      // 下一位
      count++;

      // 起始
      if (flag_start) {
        // 读取长度
        if (count >= _Frame_Len_HCHO) {
          // 重置
          count = 0;

          // 成功/完成
          buffer_done = true;
          flag_end = true;
        }
      }
    }
  }
}

bool Check() {
  uint8_t sum = 0;

  // 1~7位求和
  for (unsigned int i = 1; i < _Frame_Len_HCHO - 1; i++) {
    sum += buffer[i];
  }

  sum = (~sum) + 1;

  return sum == buffer[_Frame_Len_HCHO - 1];
}

void Read() {
  _serial_hcho.listen();
  // 串口可用
  if (_serial_hcho.isListening()) {
    unsigned int timeout = 0;
    // 容错10次
    while (buffer_done == false && timeout < 10)  //等待数据接收一帧完整
    {
      timeout++;
      // 计算
      Calc();
    }
    // 重置
    buffer_done = false;

    if (Check()) {
      // 更新数值
      data = buffer[2] * 256 + buffer[3];  // 数据定义：高八位+低八位

      // 容错交给网页处理，这里只保证在量程之内
      if (data < 0 || data > 12500) {
        data = lastData;
      } else {
        lastData = data;
      }
    }
  }
  // _serial_hcho.end();
}

}

struct _HCHO {
  void init() {
    _serial_hcho.begin(9600);
    WZ_S::passiveMode();
  }

  void read() {
    WZ_S::requestRead();
    WZ_S::Read();
  }

  int getValue() {
    return WZ_S::data;
  }

  char unit[6] = "ug\/m";
} HCHO;

}