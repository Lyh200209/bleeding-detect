#include "ADC_moniter.h"




void adc_senser::init(int senser, float gain)
{
    _senser_pin = senser;
    _gain = gain;
    _senser_value = 0;
    last_update_time = 0;
    _debug = 0;
}

void adc_senser::enable_debug(void)
{
    _debug = 1;
}

void adc_senser::disable_debug(void)
{
    _debug = 0;
}

float adc_senser::get_senser(void)
{
    return _senser_value;
}

void adc_senser::update(int interval)
{
    if (millis() - last_update_time > interval)
    {
        _senser_value = analogRead(_senser_pin);
        _senser_value = _gain*3300*_senser_value/4096;
        if (_debug)
            Serial.println(_senser_value);
        last_update_time = millis();
    }
    
}