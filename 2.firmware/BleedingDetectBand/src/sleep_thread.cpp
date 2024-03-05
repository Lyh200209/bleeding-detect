#include "sleep_thread.h"
#include "driver/rtc_io.h"



RTC_DATA_ATTR int bootCount = 0;





SLEEPThread::SLEEPThread(uint64_t mask)
{
    _mask = mask;

}

#define uart_debug
void SLEEPThread::print_wakeup_reason(void)
{
    _wakeup_cause = esp_sleep_get_wakeup_cause();
    ++bootCount;
    
    #ifdef uart_debug
    Serial.println("Boot number: " + String(bootCount));
    switch(_wakeup_cause)
    {
        case ESP_SLEEP_WAKEUP_EXT0 : 
            Serial.println("Wakeup caused by external signal using RTC_IO"); 
            break;
        case ESP_SLEEP_WAKEUP_EXT1 : 
            Serial.println("Wakeup caused by external signal using RTC_CNTL"); 
            break;
        case ESP_SLEEP_WAKEUP_TIMER : 
            Serial.println("Wakeup caused by timer"); 
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD : 
            Serial.println("Wakeup caused by touchpad"); 
            break;
        case ESP_SLEEP_WAKEUP_ULP : 
            Serial.println("Wakeup caused by ULP program"); 
            break;
        default : 
            Serial.printf("Wakeup was not caused by deep sleep: %d\n",_wakeup_cause); 
            break;
    }
    

    #endif
    if (_wakeup_cause == ESP_SLEEP_WAKEUP_EXT1)
    {
        _wake_mask = esp_sleep_get_ext1_wakeup_status();
        
        Serial.println(_wake_mask);
    }
    rtc_gpio_deinit(GPIO_NUM_32);
    rtc_gpio_deinit(GPIO_NUM_34);
    rtc_gpio_deinit(GPIO_NUM_37);
}



void SLEEPThread::ext1_enable(void)
{
    esp_sleep_enable_ext1_wakeup(_mask,ESP_EXT1_WAKEUP_ANY_HIGH);
    rtc_gpio_pullup_dis(GPIO_NUM_34);
    rtc_gpio_pullup_dis(GPIO_NUM_37);
    rtc_gpio_pulldown_en(GPIO_NUM_34);
    rtc_gpio_pulldown_en(GPIO_NUM_37);
}

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */
//2 hours
void SLEEPThread::timer_enable(void)
{
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}


void SLEEPThread::manualShutDown(void)
{
    ext1_enable();
    esp_deep_sleep_start();
}

void SLEEPThread::timerShutDown(void)
{
    ext1_enable();
    timer_enable();
    esp_deep_sleep_start();
}

void SLEEPThread::CmdShutDown(void)
{
    ext1_enable();
    esp_deep_sleep_start();
}

void SLEEPThread::refresh(void)
{
    last_update_time = millis();
}

void SLEEPThread::QuickShutDown(void)
{
    if (millis() - last_update_time > 5000)
    {
        Serial.println("self shut down");
        ext1_enable();
        timer_enable();
        esp_deep_sleep_start();
    }
}

void SLEEPThread::selfShutDown(void)
{
    if (millis() - last_update_time > 60000)
    {
        Serial.println("self shut down");
        ext1_enable();
        //timer_enable();
        esp_deep_sleep_start();
    }
}
