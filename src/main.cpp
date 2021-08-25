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
#include "Relays.h"
#include "SwampController.h"
#include "TouchScreen.h"
#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_I2CDevice.h>
#include <Adafruit_ILI9341.h>
#include <Arduino.h>
#include <SPI.h>

#define SCREEN_X_OFFSET      (10)
#define SCREEN_MOTOR_SPEED_Y (50)
#define SCREEN_PUMP_Y        (150)
#define SCREEN_HOLD_Y        (250)
#define SCREEN_VAL_X         (20)

#define BUTTON_SIZE_X               (80)
#define BUTTON_SIZE_Y               (40)
#define BUTTON_FRAME_WIDTH          (3)
#define SCREEN_MOTOR_SPEED_BUTTON_X (100)
#define SCREEN_MOTOR_SPEED_BUTTON_Y (SCREEN_MOTOR_SPEED_Y - (BUTTON_SIZE_Y / 2))
#define SCREEN_PUMP_BUTTON_X        (100)
#define SCREEN_PUMP_BUTTON_Y        (SCREEN_PUMP_Y - (BUTTON_SIZE_Y / 2))
#define SCREEN_HOLD_PLUS_BUTTON_X   (100)
#define SCREEN_HOLD_PLUS_BUTTON_Y   (SCREEN_HOLD_Y - (BUTTON_SIZE_Y / 2))
#define SCREEN_HOLD_MINUS_BUTTON_X  (100)
#define SCREEN_HOLD_MINUS_BUTTON_Y  (SCREEN_HOLD_Y - (BUTTON_SIZE_Y / 2))

#define SCREEN_BGND_COLOR      ILI9341_BLACK
#define SCREEN_TEXT_COLOR      ILI9341_WHITE
#define SCREEN_VAL_COLOR       ILI9341_YELLOW
#define SCREEN_BUTTON_FRAME    ILI9341_WHITE
#define SCREEN_BUTTON_FILL_OFF SCREEN_BGND_COLOR
#define SCREEN_BUTTON_FILL_ON  ILI9341_GREEN

#define RELAY_MOTOR_LOW_PIN  (12)
#define RELAY_MOTOR_HIGH_PIN (13)
// Size of the color selection boxes and the paintbrush size
#define RELAY_PUMP_PIN    (A1)
#define RELAY_OFF_TIME_MS (100)

// These are the four touchscreen analog pins
#define YP A2 // must be an analog pin, use "An" notation!
#define XM A3 // must be an analog pin, use "An" notation!
#define YM 9  // can be any digital pin
#define XP 8  // can be any digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// The display uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen touchScreen = TouchScreen(XP, YP, XM, YM, 300);

// LCDShield  lcd;
MotorSpeed AirMotorSpeed;
Pump       AirPump;
HoldTimer  AirHold;
Relay      RelayMotorLow(RELAY_MOTOR_LOW_PIN);
Relay      RelayMotorHigh(RELAY_MOTOR_HIGH_PIN);
Relay      RelayPump(RELAY_PUMP_PIN);

void drawButton(int16_t x, int16_t y, bool state)
{
    uint16_t fillColor;

    if (state)
    {
        fillColor = SCREEN_BUTTON_FILL_ON;
    }
    else
    {
        fillColor = SCREEN_BUTTON_FILL_OFF;
    }

    // Draw the white rectangle as outline
    tft.fillRect(x, y, BUTTON_SIZE_X, BUTTON_SIZE_Y, SCREEN_BUTTON_FRAME);
    // Draw the fill rectangle
    tft.fillRect(x + BUTTON_FRAME_WIDTH,
                 y + BUTTON_FRAME_WIDTH,
                 BUTTON_SIZE_X - (BUTTON_FRAME_WIDTH * 2),
                 BUTTON_SIZE_Y - (BUTTON_FRAME_WIDTH * 2),
                 fillColor);
}

void updateMotorSpeed(void)
{
    // lcd.setStr(AirMotorSpeed.GetText(),
    //            SCREEN_MOTOR_SPEED_X,
    //            SCREEN_VAL_Y,
    //            SCREEN_VAL_COLOR,
    //            SCREEN_BGND_COLOR);
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
    // lcd.setStr(AirPump.GetText(),
    //            SCREEN_PUMP_X,
    //            SCREEN_VAL_Y,
    //            SCREEN_VAL_COLOR,
    //            SCREEN_BGND_COLOR);
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
    // lcd.setStr(AirHold.GetText(),
    //            SCREEN_HOLD_X,
    //            SCREEN_VAL_Y,
    //            SCREEN_VAL_COLOR,
    //            SCREEN_BGND_COLOR);
    Serial.print("Hold: ");
    Serial.println(AirHold.GetText());
}

void setup()
{
    RelayMotorLow.Off();
    RelayMotorHigh.Off();
    RelayPump.Off();

    Serial.begin(9600);

    AirMotorSpeed.TurnOff();
    AirPump.TurnOff();

    updateMotorSpeed();
    updatePump();
    updateHold();

    tft.begin();
    tft.fillScreen(SCREEN_BGND_COLOR);

    tft.setTextColor(SCREEN_TEXT_COLOR);
    tft.setTextSize(2);
    tft.setCursor(SCREEN_X_OFFSET, SCREEN_MOTOR_SPEED_Y);
    tft.println("Motor:");
    drawButton(SCREEN_MOTOR_SPEED_BUTTON_X, SCREEN_MOTOR_SPEED_BUTTON_Y, false);
    tft.setCursor(SCREEN_X_OFFSET, SCREEN_PUMP_Y);
    tft.println("Pump:");
    drawButton(SCREEN_PUMP_BUTTON_X, SCREEN_PUMP_BUTTON_Y, false);
    tft.setCursor(SCREEN_X_OFFSET, SCREEN_HOLD_Y);
    tft.println("Hold:");
    drawButton(SCREEN_HOLD_MINUS_BUTTON_X, SCREEN_HOLD_MINUS_BUTTON_Y, false);
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

    // Read touchscreen
    TSPoint point = touchScreen.getPoint();
    isHoldChanged = AirHold.Update();

    // if (ButtonMotorSpeed.IsClick())
    // {
    //     AirMotorSpeed.Change();
    //     updateMotorSpeed();
    // }

    // if (ButtonPump.IsClick())
    // {
    //     AirPump.Change();
    //     updatePump();
    // }

    // if (ButtonHold.IsClick())
    // {
    //     AirHold.AddTime();
    //     isHoldChanged = true;
    // }
    // else if (ButtonHold.IsLongPress())
    // {
    //     AirHold.Stop();
    //     isHoldChanged = true;
    // }

    if (isHoldChanged)
    {
        if (AirHold.IsExpired())
        {
            turnAllOff();
        }
        updateHold();
    }
}