#include <Arduino.h>
#include "Buttons.h"

#define LONG_PRESS_TIME_MS (2000)
#define CLICK_MIN_TIME_MX  (25)

Button::Button(uint32_t pin)
{
    Pin = pin;
    pinMode(Pin, INPUT);
    digitalWrite(Pin, HIGH);
}

void Button::Read(void)
{
    CurrentState = (ButtonState_t)digitalRead(Pin);

    if (BUTTON_STATE_PRESSED == CurrentState && BUTTON_STATE_RELEASED == PrevState)
    {
        // If button is just pressed
        PressEvent     = true;
        PressTimeStamp = millis();
        HoldTime       = 0;
    }
    if (BUTTON_STATE_PRESSED == CurrentState && BUTTON_STATE_PRESSED == PrevState)
    {
        // If the button is being pressed
        HoldTime = millis() - PressTimeStamp;

        if ((LONG_PRESS_TIME_MS <= HoldTime) && PressEvent)
        {
            LongPressed = true;
        }
    }
    if (BUTTON_STATE_RELEASED == CurrentState && BUTTON_STATE_PRESSED == PrevState)
    {
        // If the button is just released
        HoldTime = millis() - PressTimeStamp;

        if (CLICK_MIN_TIME_MX < HoldTime && LONG_PRESS_TIME_MS > HoldTime)
        {
            Clicked = true;
        }
        PressEvent     = false;
        PressTimeStamp = 0;
        HoldTime       = 0;
    }
    PrevState = CurrentState;
}

ButtonState_t Button::GetState(void)
{
    return CurrentState;
}

bool Button::IsClick(void)
{
    bool result = Clicked;

    if (Clicked)
    {
        Clicked = false;
    }

    return result;
}

bool Button::IsLongPress(void)
{
    bool result = LongPressed;

    if (LongPressed)
    {
        LongPressed = false;
        PressEvent  = false;
    }

    return result;
}
