#include "SwampController.h"

const char* MotorSpeed::MotorSpeedText[SPEED_COUNT] = {"Off ", "Low ", "High"};

void MotorSpeed::Change(void)
{
    switch (Speed)
    {
        case SPEED_OFF:
            Speed = SPEED_LOW;
            break;
        case SPEED_LOW:
            Speed = SPEED_HIGH;
            break;
        case SPEED_HIGH:
        case SPEED_COUNT:
            Speed = SPEED_OFF;
            break;
    }
}

MotorSpeed_t MotorSpeed::Get(void)
{
    return Speed;
}

void MotorSpeed::Set(MotorSpeed_t speed)
{
    if (speed < SPEED_COUNT)
    {
        Speed = speed;
    }
}

char* MotorSpeed::GetText(void)
{
    return (char*)MotorSpeedText[Speed];
}

void MotorSpeed::TurnOff(void)
{
    Set(SPEED_OFF);
}

const char* Pump::PumpText[PUMP_COUNT] = {"Off", "On "};

void Pump::Change(void)
{
    switch (State)
    {
        case PUMP_OFF:
            State = PUMP_ON;
            break;
        case PUMP_ON:
        case PUMP_COUNT:
            State = PUMP_OFF;
            break;
    }
}

Pump_t Pump::Get(void)
{
    return State;
}

void Pump::Set(Pump_t state)
{
    if (state < PUMP_COUNT)
    {
        State = state;
    }
}

char* Pump::GetText(void)
{
    return (char*)PumpText[State];
}

void Pump::TurnOff(void)
{
    Set(PUMP_OFF);
}

#define MS_IN_SECOND            (1000)
#define MS_IN_MINUTE            (MS_IN_SECOND * 60)
#define MS_IN_HOUR              (MS_IN_MINUTE * 60)
#define HOLD_UPDATE_INTERVAL_MS MS_IN_MINUTE
#define HOLD_TIME_STEP_MS       (30 * MS_IN_MINUTE)
#define HOLD_TIME_MAX           (24 * MS_IN_HOUR)

bool HoldTimer::Update(void)
{
    bool isChanged = false;

    if (IsEnabled())
    {
        uint32_t timeStamp = millis();
        if (HOLD_UPDATE_INTERVAL_MS <= (timeStamp - UpdateTimeStamp))
        {
            if (HOLD_UPDATE_INTERVAL_MS > HoldTime)
            {
                HoldTime = 0;
                Stop();
                Expired = true;
            }
            else
            {
                HoldTime -= HOLD_UPDATE_INTERVAL_MS;
            }
            UpdateTimeStamp = timeStamp;
            isChanged       = true;
        }
    }

    return isChanged;
}

void HoldTimer::Start(void)
{
    HoldEnabled     = true;
    UpdateTimeStamp = millis();
    Expired         = false;
}

void HoldTimer::Stop(void)
{
    HoldEnabled = false;
    HoldTime    = 0;
}

bool HoldTimer::IsEnabled(void)
{
    return HoldEnabled;
}

char* HoldTimer::GetText(void)
{
    if (IsEnabled())
    {
        uint32_t hours   = 0;
        uint32_t minutes = 0;
        uint32_t timeMs  = HoldTime;

        hours = timeMs / MS_IN_HOUR;
        timeMs -= hours * MS_IN_HOUR;
        minutes = timeMs / MS_IN_MINUTE;
        timeMs -= -minutes * MS_IN_MINUTE;

        TimeStr[0] = (hours / 10) + '0';
        TimeStr[1] = (hours % 10) + '0';
        TimeStr[3] = (minutes / 10) + '0';
        TimeStr[4] = (minutes % 10) + '0';
    }
    else
    {
        TimeStr[0] = '-';
        TimeStr[1] = '-';
        TimeStr[3] = '-';
        TimeStr[4] = '-';
    }

    return TimeStr;
}

void HoldTimer::AddTime(void)
{
    if ((HOLD_TIME_MAX - HOLD_TIME_STEP_MS) >= HoldTime)
    {
        HoldTime += HOLD_TIME_STEP_MS;
        if (!IsEnabled())
        {
            Start();
        }
    }
}

bool HoldTimer::IsExpired(void)
{
    bool result = Expired;

    if (Expired)
    {
        Expired = false;
    }

    return result;
}