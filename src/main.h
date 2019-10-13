#ifndef MAIN_H
#define MAIN_H

// ===== ===== ==== Definitions ===== ==== ==== //
// ==== ==== Libraries ==== ====//
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
// #include <wiringPiSPI.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <math.h>
#include <bcm2835.h>

// ==== ==== Buttons ==== ====//
const int START_STOP_BUTTON = 21;
const int CHANGE_INTERVAL = 22;
const int STOP_ALARM = 23;
const int RESET_SYSTEM_TIME = 24;

// ==== ==== SPI Pins ==== ====//
const int SDA = 8;
const int SCL = 9;

// ==== ==== I2C Pins ==== ====//
const int SCLK = 14;
const int MOSI = 12;
const int MISO = 13;
const int CS0 = 0;
const int CS1 = 2;

// ==== ==== Constants ==== ====//
const int SPI_SPEED = 500000;

// ==== ====    RTC    ==== ====//
const char RTCADDR = 0x6f;
const char SEC = 0x00; // see register table in datasheet
const char MIN = 0x01;
const char HOUR = 0x02;
const char TIMEZONE = 2; // +02H00 (RSA)

// ==== ====    Start    ==== ==== //
const uint8_t start = 0x01;
const uint8_t end = 0x00;
const uint8_t chan = 0x00;

// ==== ==== Functions ==== ====//
int init_buttons(void);
// int readADC(uint8_t chan);
// float volts_adc(int adc);
int init_ADC(void);
// int init_DAC(void);
int init_RTC(void);
int update_Blynk(void);
int get_Blynk_command(void);
int change_interval(void);
long get_time(void);
float calculate_Vout(void);
int turn_off_alarm(void);
int sound_alarm(void);
int read_ADC(void);
int hFormat(int hours);
int hexCompensation(int units);
int decCompensation(int units);



#endif
