#ifndef __KEY_MONITER_H
#define __KEY_MONITER_H

#include <Arduino.h>


#define IDLE        0
#define SHORT_TRIG  1
#define LONG_PRESS  2


class KEY_MONITER
{
private:
    int _key_pin;
    int _interval;
    int flag;
    long last_update_time;
    long keep_time;
public:
    void init(int key_pin);
    int update(void);
};


#endif
