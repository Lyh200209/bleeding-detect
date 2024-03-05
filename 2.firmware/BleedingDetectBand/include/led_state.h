#ifndef __LED_STATE_H
#define __LED_STATE_H

#include <Arduino.h>

typedef enum {
    INIT        = -255,
    openning    =   0,
    shutting    =   1,
    wifi_error  =   2,
    server_error=   3,
    bat_warnning=   4,
    alarm_state =   5,
    sleep_state =   6,
}   LED_status_t;

class led_state
{
private:
    int _led_r_pin;
    int _led_g_pin;
    int _led_b_pin;
    

    int blink_time;
    int warnning_time;
    long last_update_time;

    int toggle_flag;


public:
    LED_status_t led_status;
    void init(int r_pin, int g_pin, int b_pin);
    void led_test(void);
    void set_led_status(LED_status_t status);
    void update(void);

    
};





#endif
