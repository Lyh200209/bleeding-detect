#include <Arduino.h>
#include "Passive_beep.h"

int notes1[] = {NOTE_C4,NOTE_C4,NOTE_D8,NOTE_C4};
int beats1[] = {1,1,1,1};
int length1 = 4;

int notes2[] = {NOTE_C4,NOTE_D8};
int beats2[] = {1,1};
int length2 = 2;

int tempo = 300;
float temp = 0.0;
int temp_key = 0;

PA_BUZZER::PA_BUZZER(void)
{
    _speaker_pin = 0;
    values = &temp;
    thresholds = &temp;
    listen_key = &temp_key;
}

void PA_BUZZER::initial(int pin)
{
    _speaker_pin = pin;
    pinMode(pin,OUTPUT);
}


 
void PA_BUZZER::playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(_speaker_pin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(_speaker_pin, LOW);
    delayMicroseconds(tone);
  }
}

void PA_BUZZER::playNote1(void)
{
    for (int i=0;i<length1;i++)
    {
        playTone(notes1[i], beats1[i] * tempo);
    }
    delay(4 * tempo); // rest
}

//heads up sound
void PA_BUZZER::playNote2(void)
{
    for (int i=0;i<length2;i++)
    {
        playTone(notes2[i], beats2[i] * tempo);
    }
}

void PA_BUZZER::add_warning_listenning(float *value, float *threshold)
{
    if (_speaker_pin != 0)
    {
        values = value;
        thresholds = threshold;
    }
}

void PA_BUZZER::add_key_listenning(int * key_value)
{
    if (_speaker_pin != 0)
    {
        listen_key = key_value;
    }

}

void PA_BUZZER::update(void)
{
    if (_speaker_pin != 0)
    {
        if ((*values) > (*thresholds))
        {
            playNote1();
        }
        
    }
}



