#include "key_moniter.h"

void KEY_MONITER::init(int key_pin)
{
    _key_pin = key_pin;
    pinMode(_key_pin,INPUT);
    _interval = 20;
    flag = 0;
}

int KEY_MONITER::update(void)
{
    if (millis() - last_update_time > _interval)
    {
        last_update_time = millis();
        if (flag == 0)
        {
            if (digitalRead(_key_pin))
            {
                flag = 1;
                keep_time = millis();
            }
            return IDLE;
        }
        else
        {
            if (!digitalRead(_key_pin))
            {
                flag = 0;
                if (millis() - keep_time > 3000)
                {
                    return LONG_PRESS;
                }
                else
                    return SHORT_TRIG;
            }
            else
                return IDLE;
        }
    }
    else
        return IDLE;
}


