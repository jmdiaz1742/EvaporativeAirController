/*
  TestPattern - An example sketch for the SparkFun Color LCD Shield Library
  by: Jim Lindblom
  SparkFun Electronics
  date: 6/23/11
  license: CC-BY SA 3.0 - Creative commons share-alike 3.0
  use this code however you'd like, just keep this license and
  attribute. Let me know if you make hugely, awesome, great changes.
  This sketch has example usage of the Color LCD Shield's three
  buttons. It also shows how to use the setRect and contrast
  functions.

  Hit S1 to increase the contrast, S2 decreases the contrast, and
  S3 sets the contrast back to the middle.
*/
#include <Arduino.h>
#include "ColorLCDShield.h"
#include "SwampController.h"
#include "Buttons.h"
#include "Relays.h"

#define LCD_CONTRAST         (40)
#define SCREEN_Y_OFFSET      (10)
#define SCREEN_MOTOR_SPEED_X (5)
#define SCREEN_PUMP_X        (35)
#define SCREEN_HOLD_X        (65)
#define SCREEN_VAL_Y         (75)

#define SCREEN_BGND_COLOR BLACK
#define SCREEN_TEXT_COLOR WHITE
#define SCREEN_VAL_COLOR  YELLOW

#define BUTTON_MOTOR_SPEED_PIN (5)
#define BUTTON_PUMP_PIN        (4)
#define BUTTON_HOLD_PIN        (3)

#define RELAY_MOTOR_LOW_PIN  (52)
#define RELAY_MOTOR_HIGH_PIN (50)
#define RELAY_PUMP_PIN       (48)
#define RELAY_OFF_TIME_MS    (100)

LCDShield  lcd;
MotorSpeed AirMotorSpeed;
Pump       AirPump;
HoldTimer  AirHold;
Button     ButtonMotorSpeed(BUTTON_MOTOR_SPEED_PIN);
Button     ButtonPump(BUTTON_PUMP_PIN);
Button     ButtonHold(BUTTON_HOLD_PIN);
Relay      RelayMotorLow(RELAY_MOTOR_LOW_PIN);
Relay      RelayMotorHigh(RELAY_MOTOR_HIGH_PIN);
Relay      RelayPump(RELAY_PUMP_PIN);

void updateMotorSpeed(void)
{
    lcd.setStr(AirMotorSpeed.GetText(),
               SCREEN_MOTOR_SPEED_X,
               SCREEN_VAL_Y,
               SCREEN_VAL_COLOR,
               SCREEN_BGND_COLOR);
    Serial.print("Motor: ");
    Serial.println(AirMotorSpeed.GetText());

    switch (AirMotorSpeed.Get())
    {
        case SPEED_COUNT:
        case SPEED_OFF:
            RelayMotorLow.Off();
            RelayMotorHigh.Off();
            break;
        case SPEED_LOW:
            RelayMotorHigh.Off();
            delay(RELAY_OFF_TIME_MS);
            RelayMotorLow.On();
            break;
        case SPEED_HIGH:
            RelayMotorLow.Off();
            delay(RELAY_OFF_TIME_MS);
            RelayMotorHigh.On();
            break;
    }
}

void updatePump(void)
{
    lcd.setStr(AirPump.GetText(),
               SCREEN_PUMP_X,
               SCREEN_VAL_Y,
               SCREEN_VAL_COLOR,
               SCREEN_BGND_COLOR);
    Serial.print("Pump: ");
    Serial.println(AirPump.GetText());

    switch (AirPump.Get())
    {
        case PUMP_COUNT:
        case PUMP_OFF:
            RelayPump.Off();
            break;
        case PUMP_ON:
            RelayPump.On();
            break;
    }
}

void updateHold(void)
{
    lcd.setStr(AirHold.GetText(),
               SCREEN_HOLD_X,
               SCREEN_VAL_Y,
               SCREEN_VAL_COLOR,
               SCREEN_BGND_COLOR);
    Serial.print("Hold: ");
    Serial.println(AirHold.GetText());
}

void setup()
{
    RelayMotorLow.Off();
    RelayMotorHigh.Off();
    RelayPump.Off();

    Serial.begin(9600);
    lcd.init(PHILLIPS, 1);

    lcd.contrast(LCD_CONTRAST);
    lcd.clear(SCREEN_BGND_COLOR);

    lcd.setStr(
        "Motor:", SCREEN_MOTOR_SPEED_X, SCREEN_Y_OFFSET, SCREEN_TEXT_COLOR, SCREEN_BGND_COLOR);
    lcd.setStr(
        "Pump:", SCREEN_PUMP_X, SCREEN_Y_OFFSET, SCREEN_TEXT_COLOR, SCREEN_BGND_COLOR);
    lcd.setStr(
        "Hold: ", SCREEN_HOLD_X, SCREEN_Y_OFFSET, SCREEN_TEXT_COLOR, SCREEN_BGND_COLOR);

    AirMotorSpeed.TurnOff();
    AirPump.TurnOff();

    updateMotorSpeed();
    updatePump();
    updateHold();
}

void turnAllOff(void)
{
    AirMotorSpeed.TurnOff();
    AirPump.TurnOff();

    updateMotorSpeed();
    updatePump();
}

void loop()
{
    bool isHoldChanged = false;

    ButtonMotorSpeed.Read();
    ButtonPump.Read();
    ButtonHold.Read();
    isHoldChanged = AirHold.Update();

    if (ButtonMotorSpeed.IsClick())
    {
        AirMotorSpeed.Change();
        updateMotorSpeed();
    }

    if (ButtonPump.IsClick())
    {
        AirPump.Change();
        updatePump();
    }

    if (ButtonHold.IsClick())
    {
        AirHold.AddTime();
        isHoldChanged = true;
    }
    else if (ButtonHold.IsLongPress())
    {
        AirHold.Stop();
        isHoldChanged = true;
    }

    if (isHoldChanged)
    {
        if (AirHold.IsExpired())
        {
            turnAllOff();
        }
        updateHold();
    }
}