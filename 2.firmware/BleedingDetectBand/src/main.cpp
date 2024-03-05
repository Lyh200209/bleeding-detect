#include <Arduino.h>
#include "WCC_PicoMqtt.h"
#include "ADC_moniter.h"
#include "key_moniter.h"
#include "led_state.h"
#include "Passive_beep.h"
#include "sleep_thread.h"

RTC_DATA_ATTR String server = "172.20.10.6";
RTC_DATA_ATTR int port = 1883;
WCC_MQTT esp32MQTT(server,port);


RTC_DATA_ATTR String ssid = "iPhone";
RTC_DATA_ATTR String password = "12345678";
RTC_DATA_ATTR String USER = "bed_NO.001";


#define BUTTON_PIN_BITMASK 0x2500000000
SLEEPThread sleepthread(BUTTON_PIN_BITMASK);

PA_BUZZER buzzer;



float WaterValue;
float WaterThreshold = 1000.0;

float POWERValue;
float POWERThreshold = 3300.0;

adc_senser WaterAdc;
adc_senser PowerAdc;

led_state led;
LED_status_t last_led_state;

int key_status = 0;
KEY_MONITER key;


void beep_loop(void *p)
{
  buzzer.add_warning_listenning(&WaterValue ,&WaterThreshold);
  while(1)
  {
    delay(500);
    buzzer.update();
  }
}

void alarm_loop(void *p)
{
  while(1)
  {
    buzzer.playNote1();
    delay(500);
  }
}

void alarm_mqtt_loop(void *p)
{
  esp32MQTT.initial();
  esp32MQTT.WiFi_connect(ssid,password);
  esp32MQTT.MQTT_connect();
  esp32MQTT.set_user(USER);

  esp32MQTT.add_topic("bleeding_topic");
  esp32MQTT.subscribe_all_topic();

  while(1)
  {
    esp32MQTT.update();
    esp32MQTT.publish_cmd("bleeding_topic", "AlarmOn");
    delay(2000);
  }
}


void manual_mqtt_loop(void *p)
{
  while (1)
  {
    delay(20);
    esp32MQTT.update();
    
  }
}



#define network_moniter
#define power_moniter

void timer_loop_moniter(void *p)
{

  esp32MQTT.initial();
  esp32MQTT.WiFi_connect(ssid,password);
  esp32MQTT.MQTT_connect();
  esp32MQTT.set_user(USER);
  
  esp32MQTT.add_listenning_cmd(&WaterValue, &WaterThreshold, "bleeding_topic","AlarmOn");
  esp32MQTT.add_topic("power_topic");

  if(esp32MQTT._sta_status == WCC_SERVER_CONNECTED)
  {
    esp32MQTT.subscribe_all_topic();
    esp32MQTT.publish_cmd("bleeding_topic", "IntimeOnline");
    esp32MQTT.publish_cmd("power_topic", "IntimeOnline");
  }
  WaterAdc.init(32,1.0);
  PowerAdc.init(33,6.92);
  String  adc_value = String(PowerAdc.get_senser());

  esp32MQTT.publish_cmd("power_topic",adc_value.c_str());

  delay(200);

  sleepthread.refresh();
  //启动蜂鸣器进程
  xTaskCreate(beep_loop,"beep_LOOP",10240,NULL,2,NULL);

  while (1)
  {
    //manual 启动
    delay(20);
    esp32MQTT.update();
    WaterAdc.update(1000);
    WaterValue = WaterAdc.get_senser();

    key_status = key.update();

    last_led_state = led.led_status;
    if (last_led_state == alarm_state)
    {
      if (key_status == SHORT_TRIG)
      {
        led.set_led_status(openning);
      }
      else  if (key_status == LONG_PRESS)
      {
      
        led.set_led_status(shutting);
      }
    }
    

    if ((last_led_state == shutting) && (led.led_status == INIT))
    {
      sleepthread.manualShutDown();
    }
    if (WaterValue >= WaterThreshold)
    {
      led.set_led_status(alarm_state);
      sleepthread.refresh();
    }

    led.update();
    sleepthread.QuickShutDown();

  }
}




void setup() {

  Serial.begin(115200); // open the serial port at 115200 bps;
	delay(100);


  led.init(23,22,25);
  buzzer.initial(12);
  key.init(36);


  sleepthread.print_wakeup_reason();
  if(sleepthread._wakeup_cause == ESP_SLEEP_WAKEUP_TIMER)
  {
      //执行定时监测程序
      
      //xTaskCreate(timer_loop_moniter,"loop_moniter",10240,NULL,1,NULL);

      //sleepthread.timerShutDown();
  }
  else  {
    if (sleepthread._wake_mask == 0x100000000)
    {
      //由ADC激活
      led.set_led_status(alarm_state);

      xTaskCreate(alarm_mqtt_loop,"alarm_mqtt_LOOP",10240,NULL,1,NULL);
      xTaskCreate(alarm_loop,"alarm_LOOP",10240,NULL,2,NULL);
    }
    else if (sleepthread._wake_mask == 0x400000000)
    {
      //SPI 线程 无需启动mqtt等高功耗进程
      //在后面编写SPI服务程序

    }
    else if (sleepthread._wake_mask == 0x2000000000)
    {
      //manual 启动
      led.set_led_status(openning);
      while (led.led_status != INIT)
      {
        led.update();
      }
      
      esp32MQTT.initial();
      esp32MQTT.WiFi_connect(ssid,password);
      esp32MQTT.MQTT_connect();
      esp32MQTT.set_user(USER);
      esp32MQTT.add_listenning_cmd(&WaterValue, &WaterThreshold, "bleeding_topic","AlarmOn");
      esp32MQTT.add_listenning_cmd(&POWERThreshold, &POWERValue, "power_topic", "LowerPower");
      if(esp32MQTT._sta_status == WCC_SERVER_CONNECTED)
      {
      
        esp32MQTT.subscribe_all_topic();
        esp32MQTT.publish_cmd("bleeding_topic", "HardwareOnline");
        esp32MQTT.publish_cmd("power_topic", "HardwareOnline");
      }
      
      WaterAdc.init(32,1.0);
      //WaterAdc.enable_debug();
      PowerAdc.init(33,6.92);
      sleepthread.refresh();

      //WaterValue = 3300*analogRead(32)/4096.0;
      delay(200);

      //启动蜂鸣器进程
      xTaskCreate(manual_mqtt_loop,"MQTT_LOOP",10240,NULL,1,NULL);
      xTaskCreate(beep_loop,"beep_LOOP",10240,NULL,2,NULL);
    }
  }

}

void loop() {

while (1)
{
  delay(20);
  if (sleepthread._wake_mask == 0x100000000)
  {
    //由ADC激活
    key_status = key.update();


    if (key_status == SHORT_TRIG)
    {
      led.set_led_status(shutting);
    }
    else  if (key_status == LONG_PRESS)
    {
      
      led.set_led_status(shutting);
    }
    led.update();

    if (led.led_status == sleep_state)
    {
      Serial.println("good bye");
      sleepthread.manualShutDown();
    }

  }
  else if (sleepthread._wake_mask == 0x400000000)
  {
      //SPI 线程 无需启动mqtt等高功耗进程
      //在后面编写SPI服务程序

  }
  else if (sleepthread._wake_mask == 0x2000000000)
  {
    //manual 启动
    WaterAdc.update(1000);
    WaterValue = WaterAdc.get_senser();

    PowerAdc.update(2000);
    POWERValue = PowerAdc.get_senser();
    //esp32MQTT.update();
    key_status = key.update();

    if (led.led_status == sleep_state)
    {
      Serial.println("good bye");
      sleepthread.manualShutDown();
    }
    else if (key_status == LONG_PRESS)
    {
      led.set_led_status(shutting);
    }
    else if (WaterValue>WaterThreshold)
    {
      Serial.println(WaterValue);
      sleepthread.refresh();
      led.set_led_status(alarm_state);
    }
    else  if (key_status == SHORT_TRIG)
    {
      if (POWERValue<POWERThreshold)
      {
        
        led.set_led_status(bat_warnning);
      }
      else
      {
        led.set_led_status(openning);
      }
    }
    else  if (led.led_status== INIT &&((esp32MQTT._sta_status == WCC_CONNECTION_LOST) ||
              (esp32MQTT._sta_status == WCC_DISCONNECTED) ||
              (esp32MQTT._sta_status == WCC_CONNECT_FAILED)))
    {
        
        led.set_led_status(wifi_error);
    }
    else  if (led.led_status== INIT &&((esp32MQTT._sta_status == WCC_SERVER_ERROR) ||
            (esp32MQTT._sta_status == WCC_SERVER_LOST)))
    {
       
      led.set_led_status(server_error);
    }
    
    led.update();

    
    sleepthread.selfShutDown();
  }
  else
  {
    sleepthread.manualShutDown();
  }
  
}
}



