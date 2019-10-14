#ifndef MAIN_H
#define MAIN_H

// ===== ===== ==== Definitions ===== ==== ==== //
// ==== ==== Libraries ==== ====//
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include <pthread.h>
// #include <bcm2835.h>

<<<<<<< HEAD
=======
//blynk defines
#define BLYNK_PRINT stdout
#ifdef RASPBERRY
  #include <BlynkApiWiringPi.h>
#else
  #include <BlynkApiLinux.h>
#endif
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>
#include <BlynkWidgets.h>

//blynk buttons
int START_STOP_BUTTON_BLYNK = 5; //GPIO5 Pin on the Pi
int CHANGE_INTERVAL_BLYNK = 6; //GPIO6 Pin on the Pi
int STOP_ALARM_BLYNK = 13; //GPIO13 Pin on the Pi
int RESET_SYSTEM_TIME_BLYNK = 19; //GPIO19 Pin on the Pi


>>>>>>> daa2c54a3a6930e23e2674bec657036f997f2795
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

// ==== ==== Constants ==== ====//
const int SPI_SPEED = 500000;
const int DEBOUNCE_TIME = 300;
const int ALARM_WAIT = 1000  * 10;

// ==== ====    RTC    ==== ====//
const char RTCADDR = 0x6f;
const char SEC = 0x00; // see register table in datasheet
const char MIN = 0x01;
const char HOUR = 0x02;
const char TIMEZONE = 2; // +02H00 (RSA)

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} tyme;

// ==== ==== Functions ==== ====//
int init_buttons(void);
int init_ADC(void);
int init_DAC(void);
int init_RTC(void);
int update_Blynk(void);
int get_Blynk_command(void);
long get_time(void);
float calculate_Vout(void);
int sound_alarm(void);
int read_ADC(void);
int hFormat(int hours);
void toggle_monitor(void);
void toggle_interval(void);
void reset_time(void);
void turn_off_alarm(void);
void toggle_monitor_isr(void);
void toggle_interval_isr(void);
void reset_time_isr(void);
void turn_off_alarm_isr(void);
int hexCompensation(int units);
int print_heading(void);
int decCompensation(int units);
tyme time_difference(tyme a, tyme b);
tyme now(void);
void wait(void);

#endif
