#ifndef SWAMP_CONTROLLER_H
#define SWAMP_CONTROLLER_H

#include <Arduino.h>

typedef enum
{
    SPEED_OFF = 0,
    SPEED_LOW,
    SPEED_HIGH,

    SPEED_COUNT
} MotorSpeed_t;

class MotorSpeed
{
  private:
    static const char* MotorSpeedText[SPEED_COUNT];

    MotorSpeed_t Speed = SPEED_OFF;

  public:
    void         Change(void);
    MotorSpeed_t Get(void);
    void         Set(MotorSpeed_t speed);
    char*        GetText(void);
    void         TurnOff(void);
};

typedef enum
{
    PUMP_OFF = 0,
    PUMP_ON,

    PUMP_COUNT
} Pump_t;

class Pump
{
  private:
    static const char* PumpText[PUMP_COUNT];

    Pump_t State = PUMP_OFF;

  public:
    void   Change(void);
    Pump_t Get(void);
    void   Set(Pump_t state);
    char*  GetText(void);
    void   TurnOff(void);
};

class HoldTimer
{
  private:
    bool     HoldEnabled     = false;
    bool     Expired         = false;
    uint32_t HoldTime        = 0;
    uint32_t UpdateTimeStamp = 0;
    char     TimeStr[6]      = "--:--";

  public:
    bool  Update(void);
    void  Start(void);
    void  Stop(void);
    bool  IsEnabled(void);
    char* GetText(void);
    void  AddTime(void);
    bool  IsExpired(void);
};

#endif // SWAMP_CONTROLLER_H