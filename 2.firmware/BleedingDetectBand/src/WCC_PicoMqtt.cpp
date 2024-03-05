#include "WCC_PicoMqtt.h"
#include "ArduinoJson.h"
#include <string.h>
#include <Arduino.h>
#include <WiFi.h>

#define __MQTT_Serial_Debug

extern RTC_DATA_ATTR String server ;
extern RTC_DATA_ATTR int port ;

extern RTC_DATA_ATTR String ssid ;
extern RTC_DATA_ATTR String password ;
extern RTC_DATA_ATTR String USER ;

const static int RecBuffLength = 100;
char WCC_MQTT_RecBuff[RecBuffLength] = {0}; // 接收缓存区

int WCC_MQTT_RecMsgFlag = 0;


String my_WiFi_ssid;
String my_WiFi_password;
String my_MQTT_user;

WiFiClient MyWifi_client;
PubSubClient MyMQTT_client(MyWifi_client); // 创建一个MQTT客户端

int WCC_MQTT_RecMsg(void)
{
    return WCC_MQTT_RecMsgFlag;
}
void WCC_MQTT_ClearRecMsg(void)
{
    WCC_MQTT_RecMsgFlag = 0;
}

void WCC_MQTT_RecCallback(char *topic, byte *payload, unsigned int length)
{
    memset(WCC_MQTT_RecBuff,'\0',sizeof(WCC_MQTT_RecBuff));//清空buff
    for (int i = 0; i < length && i < RecBuffLength; i++) // 使用循环打印接收到的信息
    {
        WCC_MQTT_RecBuff[i] = (char)payload[i];
        WCC_MQTT_RecMsgFlag = 1;
    }
    #ifdef __MQTT_Serial_Debug
        Serial.print("This topic:"); // 串口打印：来自订阅的主题:
        Serial.println(topic);
        Serial.print("message:");                             // 串口打印：信息：
    
        
        Serial.println(WCC_MQTT_RecBuff);
    #endif
}

WCC_MQTT::WCC_MQTT(String server, int port)
{
    mtqx_server = server;
    clientId = "esp32-" + WiFi.macAddress();
    MQTT_port = port;
    topic_num = 0;
    listening_monitor = 0;

}



void WCC_MQTT::initial(void)
{
    MyMQTT_client.setServer(mtqx_server.c_str(), (uint16_t)MQTT_port);
    MyMQTT_client.setBufferSize(4096);
    MyMQTT_client.setCallback(WCC_MQTT_RecCallback);
    _sta_status = WCC_INITED;
}


void WCC_MQTT::MQTT_connect(void)
{
    if (_sta_status == WCC_CONNECTED)
    {
    #ifdef __MQTT_Serial_Debug
    Serial.println("client server..."); // 串口打印：连接服务器中
    #endif
    last_update_time = millis();
    while (MyMQTT_client.connected() == false)
    {
        if (MyMQTT_client.connect(clientId.c_str()) == true)
        {
            #ifdef __MQTT_Serial_Debug
            Serial.println("server ok!"); // 串口打印：服务器连接成功
            #endif
            _sta_status = WCC_SERVER_CONNECTED;
        }
        if (millis() - last_update_time > server_timeout)
        {
            Serial.print("Server fault");          // 串口打印：连接服务器失败
            Serial.println(MyMQTT_client.state()); // 重新连接函数
            _sta_status = WCC_SERVER_ERROR;
            break;
        }
        #ifdef __MQTT_Serial_Debug
        if ((millis() - last_update_time) % debug_timeout > debug_timeout)
        {
            Serial.print("Server fault");          // 串口打印：连接服务器失败
            Serial.println(MyMQTT_client.state()); // 重新连接函数
        }
        #endif
    }
    }
}

void WCC_MQTT::WiFi_connect(String ssid ,String password)
{
    #ifdef __MQTT_Serial_Debug
    Serial.println("Connecting.. "); // 串口打印：连接wifi中
    #endif
    

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    #ifdef __MQTT_Serial_Debug
    Serial.print("Wait for WiFi... ");
    #endif

    last_update_time = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - last_update_time > wifi_timeout)
        {
            _sta_status = WCC_CONNECT_FAILED;
            Serial.println("Wfi failed");
            break;
        }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        _sta_status = WCC_CONNECTED;
        #ifdef __MQTT_Serial_Debug
            Serial.println("");
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        #endif
    }
}

void WCC_MQTT::add_topic(String topic)
{
    if (!topic.isEmpty())
    {
        topic_buff[topic_num] = topic;
        topic_num++;
    }
}

void WCC_MQTT::subscribe_all_topic(void)
{
    if (_sta_status == WCC_SERVER_CONNECTED)
    {
        for (int i = 0;i<topic_num;i++)
        {
            MyMQTT_client.subscribe(topic_buff[i].c_str());
        }
    }

}

void WCC_MQTT::topic_subscribe(const char* topic)
{
    if (_sta_status == WCC_SERVER_CONNECTED)
    {
        MyMQTT_client.subscribe(topic);
    }
}

void WCC_MQTT::publish(const char* topic ,const char* message)
{
    if (_sta_status == WCC_SERVER_CONNECTED)
    {
        MyMQTT_client.publish(topic,message);
    }
}

void WCC_MQTT::publish_all(const char* message)
{
    if (_sta_status == WCC_SERVER_CONNECTED)
    {
        for (int i = 0;i<topic_num;i++)
        {
            MyMQTT_client.publish(topic_buff[i].c_str(),message);
        }
    }
}

void WCC_MQTT::publish_cmd(const char* topic ,const char* cmd)
{
    if (_sta_status == WCC_SERVER_CONNECTED)
    {
        DynamicJsonDocument doc(1024);
        doc["id"] = my_MQTT_user.c_str();
        doc["cmd"] = cmd;
        char MqttSendBuff[1024];
        serializeJson(doc, MqttSendBuff);
        MyMQTT_client.publish(topic,MqttSendBuff);
    }

}

void WCC_MQTT::add_listenning(float *value, float *threshold, String topic, String message)
{
    if (listening_monitor == 10)
    {
        return;
    }
    else{
        add_topic(topic);
        listening_topics[listening_monitor] = topic;
        values[listening_monitor] = value;
        thresholds[listening_monitor] = threshold;
        messages[listening_monitor] = message;
        listenig_update_times[listening_monitor] = 0;
        listening_monitor ++;
        
    }
}

void WCC_MQTT::add_listenning_cmd(float *value, float *threshold, String topic, const char* cmd)
{
    if (listening_monitor == 10)
    {
        return;
    }
    else{
        add_topic(topic);
        listening_topics[listening_monitor] = topic;
        values[listening_monitor] = value;
        thresholds[listening_monitor] = threshold;

        DynamicJsonDocument doc(1024);
        doc["id"] = my_MQTT_user.c_str();
        doc["cmd"] = cmd;
        char MqttSendBuff[1024];
        serializeJson(doc, MqttSendBuff);


        messages[listening_monitor] = String(MqttSendBuff);
        listenig_update_times[listening_monitor] = 0;
        listening_monitor ++;
        
    }
}

void WCC_MQTT::update(void)
{
    if (_sta_status == WCC_SERVER_CONNECTED)
    {
        for (int i ;i<listening_monitor;i++)
        {
            if (((*values[i]) > (*thresholds[i])) && (millis() - listenig_update_times[i] > MQTT_timeout))
            {
                MyMQTT_client.publish(listening_topics[i].c_str(), messages[i].c_str());
                listenig_update_times[i] = millis();
            }
        }
        if (WiFi.status() != WL_CONNECTED)
        {
            _sta_status = WCC_CONNECTION_LOST;
        }
        if (MyMQTT_client.connected() == false)
        {
            _sta_status = WCC_SERVER_LOST;
        }
        MyMQTT_client.loop();
    }
    if (_sta_status == WCC_CONNECT_FAILED)
    {
        WiFi_connect(ssid,password);
    }
    if (_sta_status == WCC_CONNECTION_LOST)
    {
        #ifdef __MQTT_Serial_Debug
        Serial.println("Connecting.. "); // 串口打印：连接wifi中
        #endif
        last_update_time = millis();


        #ifdef __MQTT_Serial_Debug
        Serial.print("Wait for WiFi... ");
        #endif

        while (WiFi.status() != WL_CONNECTED)
        {
            if (millis() - last_update_time > wifi_timeout)
            {
                _sta_status = WCC_CONNECT_FAILED;
                break;
            }
            #ifdef __MQTT_Serial_Debug
            if ((millis() - last_update_time) % debug_timeout > debug_timeout)
            {
                Serial.print(".");
            }
            #endif
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            _sta_status = WCC_CONNECTED;
            #ifdef __MQTT_Serial_Debug
                Serial.println("");
                Serial.println("WiFi connected");
                Serial.print("IP address: ");
                Serial.println(WiFi.localIP());
            #endif
        }
    }
    if ((_sta_status == WCC_CONNECTED) || (_sta_status == WCC_SERVER_LOST))
    {
        if (MyMQTT_client.connected() == false)
        {
            last_update_time = millis();
            while (MyMQTT_client.connected() == false)
            {
                String clientId = "esp32-" + WiFi.macAddress();
                if (MyMQTT_client.connect(clientId.c_str()) == true)
                {
                    _sta_status = WCC_SERVER_CONNECTED;
                }
                else
                {
                    
                    if (millis() - last_update_time > server_timeout)
                    {
                        _sta_status = WCC_SERVER_ERROR;
                        Serial.println("failed");
                        Serial.println(MyMQTT_client.state()); // 重连
                        Serial.println("try again");
                        break;
                    } 
                }
            }
            if (_sta_status == WCC_SERVER_CONNECTED)
            {
                subscribe_all_topic();
                DynamicJsonDocument doc(1024);
                doc["id"] = my_MQTT_user.c_str();
                doc["cmd"] = "HardwareOnline";
                char MqttSendBuff[1024];
                serializeJson(doc, MqttSendBuff);
                publish_all(MqttSendBuff);
            }
        }
    }
}


void WCC_MQTT::WiFi_off(void)
{
    WiFi.disconnect(true,true);
    WiFi.mode(WIFI_OFF);
}

void WCC_MQTT::WiFi_reconfig(const char* ssid ,const char* password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    #ifdef __MQTT_Serial_Debug
    Serial.print("Wait for WiFi... ");
    #endif

    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - last_update_time > wifi_timeout)
        {
            _sta_status = WCC_CONNECT_FAILED;
            break;
        }
        #ifdef __MQTT_Serial_Debug
        if ((millis() - last_update_time) % debug_timeout > debug_timeout)
        {
            Serial.print(".");
        }
        #endif
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        _sta_status = WCC_CONNECTED;
        #ifdef __MQTT_Serial_Debug
            Serial.println("");
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        #endif
    }

}

void WCC_MQTT::set_user(String user)
{
    my_MQTT_user = user;
}
