#include "led_state.h"


void led_state::init(int r_pin, int g_pin, int b_pin)
{
    _led_r_pin = r_pin;
    _led_g_pin = g_pin;
    _led_b_pin = b_pin;
    pinMode(_led_r_pin ,OUTPUT);
    pinMode(_led_g_pin ,OUTPUT);
    pinMode(_led_b_pin, OUTPUT);

    digitalWrite(_led_r_pin, LOW);
    digitalWrite(_led_g_pin, LOW);
    digitalWrite(_led_b_pin, LOW);

    led_status = INIT;
    blink_time = 250;
    warnning_time = 250;
    last_update_time = millis();
}




void led_state::led_test(void)
{
    digitalWrite(_led_r_pin, HIGH);
    digitalWrite(_led_g_pin, LOW);
    digitalWrite(_led_b_pin, LOW);
    delay(500);

    digitalWrite(_led_r_pin, LOW);
    digitalWrite(_led_g_pin, HIGH);
    digitalWrite(_led_b_pin, LOW);
    delay(500);

    digitalWrite(_led_r_pin, LOW);
    digitalWrite(_led_g_pin, LOW);
    digitalWrite(_led_b_pin, HIGH);
    delay(500);

    digitalWrite(_led_r_pin, HIGH);
    digitalWrite(_led_g_pin, HIGH);
    digitalWrite(_led_b_pin, LOW);
    delay(500);

    digitalWrite(_led_r_pin, HIGH);
    digitalWrite(_led_g_pin, LOW);
    digitalWrite(_led_b_pin, HIGH);
    delay(500);

    digitalWrite(_led_r_pin, LOW);
    digitalWrite(_led_g_pin, HIGH);
    digitalWrite(_led_b_pin, HIGH);
    delay(500);

    digitalWrite(_led_r_pin, HIGH);
    digitalWrite(_led_g_pin, HIGH);
    digitalWrite(_led_b_pin, HIGH);
    delay(500);

}

void led_state::set_led_status(LED_status_t status)
{
    if (status == openning)
    {
        led_status = status;
        toggle_flag = 1;
        digitalWrite(_led_g_pin, HIGH);
        digitalWrite(_led_r_pin, LOW);
        digitalWrite(_led_b_pin, LOW);
        last_update_time = millis();
    }
    else    if (status == shutting)
    {
        led_status = status;
        toggle_flag = 1;
        digitalWrite(_led_r_pin, HIGH);
        digitalWrite(_led_g_pin, LOW);
        digitalWrite(_led_b_pin, LOW);
        last_update_time = millis();
    }
    else    if (status == wifi_error)
    {
        led_status = status;
        digitalWrite(_led_r_pin, HIGH);
        digitalWrite(_led_g_pin, HIGH);
        digitalWrite(_led_b_pin, LOW);
    }
    else    if (status == server_error)
    {
        led_status = status;
        digitalWrite(_led_r_pin, HIGH);
        digitalWrite(_led_g_pin, LOW);
        digitalWrite(_led_b_pin, HIGH);
    }
    else    if (status == bat_warnning)
    {
        led_status = status;
        digitalWrite(_led_r_pin, HIGH);
        digitalWrite(_led_g_pin, LOW);
        digitalWrite(_led_b_pin, LOW);
    }
    else    if (status == alarm_state)
    {
        led_status = status;
        toggle_flag = 1;
        digitalWrite(_led_r_pin, HIGH);
        digitalWrite(_led_g_pin, LOW);
        digitalWrite(_led_b_pin, LOW);
        last_update_time = millis();
    }
    else
    {
        led_status = INIT;
        digitalWrite(_led_r_pin, LOW);
        digitalWrite(_led_g_pin, LOW);
        digitalWrite(_led_b_pin, LOW);

    }
}


//update 均为无时延功能函数
void led_state::update(void)
{
    //底层驱动程序
    if (led_status == openning)
    {
        if (millis() - last_update_time > blink_time)
        {
            
            if (toggle_flag == 1)
            {
                digitalWrite(_led_g_pin, LOW);
                last_update_time = millis();
                toggle_flag = 2;
            }
            else if (toggle_flag == 2)
            {
                digitalWrite(_led_g_pin, HIGH);
                last_update_time = millis();
                toggle_flag = 3;
            }
            else if (toggle_flag == 3)
            {
                digitalWrite(_led_g_pin, LOW);
                last_update_time = millis();
                toggle_flag = 0;
                led_status = INIT;
                digitalWrite(_led_r_pin, LOW);
                digitalWrite(_led_g_pin, LOW);
                digitalWrite(_led_b_pin, LOW);
            }
        }
    }
    else if (led_status == shutting)
    {
        if (millis() - last_update_time > blink_time)
        {
            if (toggle_flag == 1)
            {
                digitalWrite(_led_r_pin, LOW);
                last_update_time = millis();
                toggle_flag = 2;
            }
            else if (toggle_flag == 2)
            {
                digitalWrite(_led_r_pin, HIGH);
                last_update_time = millis();
                toggle_flag = 3;
            }
            else if (toggle_flag == 3)
            {
                digitalWrite(_led_r_pin, LOW);
                last_update_time = millis();
                toggle_flag = 0;
                led_status = sleep_state;
                digitalWrite(_led_r_pin, LOW);
                digitalWrite(_led_g_pin, LOW);
                digitalWrite(_led_b_pin, LOW);
            }
        }
    }
    else    if (led_status == alarm_state)
    {
        if (millis() - last_update_time > warnning_time)
        {
            if (toggle_flag == 1)
            {
                digitalWrite(_led_r_pin, LOW);
                last_update_time = millis();
                toggle_flag = 2;
            }
            else    if (toggle_flag == 2)
            {
                digitalWrite(_led_r_pin, HIGH);
                last_update_time = millis();
                toggle_flag = 1;
            }
        }
    }


}