#include <Arduino.h>

#define _Pin_Btn_1 12

namespace Buttons {

enum Status {
  Ready = 0,
  Down = 1,
  Up = 2
};

struct _Btn_1 {
  Status status = Ready;

  void init() {
    pinMode(_Pin_Btn_1, INPUT_PULLUP);
  }

  void loop() {
    if (status == Ready && digitalRead(_Pin_Btn_1) == LOW) {
      status = Down;
    }

    if (status != Ready && digitalRead(_Pin_Btn_1) == HIGH) {
      status = Up;
    }
  }

  bool getValue() {
    bool result = status == Up;

    if (result) {
      status = Ready;
    }
    return result;
  }
} Btn_1;

}