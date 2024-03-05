#ifndef __ADC_MONITER_H
#define __ADC_MONITER_H

#include <Arduino.h>

class adc_senser
{
private:
    int _senser_pin;
    float _gain;
    float _senser_value;
    long last_update_time;
    int _debug;


public:
    void init(int senser, float gain = 1.0f);
    float get_senser(void);
    void enable_debug(void);
    void disable_debug(void);
    void update(int interval);


};


#endif
