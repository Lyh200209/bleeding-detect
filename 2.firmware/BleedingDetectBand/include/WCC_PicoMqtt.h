#ifndef __WCC_PICOMQTT_H
#define __WCC_PICOMQTT_H

#include <string.h>
#include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include "ArduinoJson.h"

typedef enum {
    WCC_NO_SHIELD           = 255,   // for compatibility with WiFi Shield library   // for compatibility with WiFi Shield library
    WCC_IDLE_STATUS         = 0,
    WCC_NO_SSID_AVAIL       = 1,
    WCC_SCAN_COMPLETED      = 2,
    WCC_CONNECTED           = 3,
    WCC_CONNECT_FAILED      = 4,
    WCC_CONNECTION_LOST     = 5,
    WCC_DISCONNECTED        = 6,
    WCC_INITED              = 7, 
    WCC_SERVER_ERROR        = 8,
    WCC_SERVER_CONNECTED    = 9,
    WCC_SERVER_LOST         =10
}   MQTT_status_t;


void WCC_MQTT_RecCallback(char *topic, byte *payload, unsigned int length);


class WCC_MQTT
{
private:
    String mtqx_server;
    String clientId;
    int MQTT_port;

    int topic_num;
    String topic_buff[10];


    long    last_update_time;
    long    server_timeout = 5000;
    long    wifi_timeout = 5000;

    long    debug_timeout = 500;

    int     listening_monitor;
    String  listening_topics[10];
    float   *values[10];
    float   *thresholds[10];
    String  messages[10];
    long    listenig_update_times[10];
    long    MQTT_timeout = 2000;


public:
    MQTT_status_t _sta_status;
    WCC_MQTT(String server, int port);
    void initial(void);
    
    void MQTT_connect(void);
    void WiFi_connect(String ssid ,String password);

    void set_user(String user);

    void add_topic(String topic);
    void subscribe_all_topic(void);
    void topic_subscribe(const char* topic);
    
    void publish(const char* topic ,const char* message);
    void publish_all(const char* message);
    void publish_cmd(const char* topic ,const char* cmd);

    void add_listenning(float *value, float *threshold, String topic, String message);
    void add_listenning_cmd(float *value, float *threshold, String topic, const char* cmd);


    void update(void);

    void WiFi_off(void);
    void WiFi_reconfig(const char* ssid ,const char* password);



};



#endif
