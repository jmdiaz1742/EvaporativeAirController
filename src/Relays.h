#ifndef RELAYS_H
#define REALYS_H

#include <Arduino.h>

typedef enum
{
    RELAY_STATE_OFF = LOW,
    RELAY_STATE_ON  = HIGH,

    RELAY_STATE_COUNT
} RelayState_t;

class Relay
{
  private:
    RelayState_t State = RELAY_STATE_OFF;
    uint32_t     Pin;

    void SetState(RelayState_t state);

  public:
    Relay(uint32_t pin);
    RelayState_t GetState(void);
    void         On(void);
    void         Off(void);
};

#endif // REALYS_H