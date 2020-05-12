#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

typedef enum
{
    BUTTON_MOTOR_SPEED = 0,
    BUTTON_PUMP,
    BUTTON_HOLD,

    BUTTON_COUNT
} Button_t;

typedef enum
{
    BUTTON_STATE_PRESSED  = LOW,
    BUTTON_STATE_RELEASED = HIGH,

    BUTTON_STATE_COUNT
} ButtonState_t;

class Button
{
  private:
    uint32_t      Pin;
    ButtonState_t PrevState      = BUTTON_STATE_RELEASED;
    ButtonState_t CurrentState   = BUTTON_STATE_RELEASED;
    uint32_t      PressTimeStamp = 0;
    uint32_t      HoldTime       = 0;
    bool          Clicked        = false;
    bool          LongPressed    = false;
    bool          PressEvent     = false;

  public:
    Button(uint32_t pin);
    void          Read(void);
    ButtonState_t GetState(void);
    bool          IsClick(void);
    bool          IsLongPress(void);
};

#endif // BUTTONS_H