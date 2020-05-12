#include "Relays.h"

Relay::Relay(uint32_t pin)
{
    Pin = pin;
    pinMode(Pin, OUTPUT);
}

RelayState_t Relay::GetState(void)
{
    return State;
}

void Relay::SetState(RelayState_t state)
{
    if (state < RELAY_STATE_COUNT)
    {
        State = state;
    }
    digitalWrite(Pin, State);
}

void Relay::On(void)
{
    SetState(RELAY_STATE_ON);
}

void Relay::Off(void)
{
    SetState(RELAY_STATE_OFF);
}
