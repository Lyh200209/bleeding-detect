#ifndef __SLEEP_THREAD_H
#define __SLEEP_THREAD_H

#include <Arduino.h>



class SLEEPThread
{
private:
    
    uint64_t _mask;
    
    
    long last_update_time;


public:

    esp_sleep_wakeup_cause_t _wakeup_cause;
    uint64_t _wake_mask;
    SLEEPThread(uint64_t mask);
    void print_wakeup_reason(void);
    void ext1_enable(void);
    void timer_enable(void);

    void manualShutDown(void);
    void timerShutDown(void);

    void CmdShutDown(void);

    void refresh(void);
    void QuickShutDown(void);
    void selfShutDown(void);

};




#endif
