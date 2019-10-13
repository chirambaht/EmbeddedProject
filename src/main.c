#include "main.h"

// ===== ===== ==== Variable Definitions ===== ==== ==== //
int monitor = 0;                // 1 = on, 0 = off
int interval = 1;               // How often values will be logged
long last_alarm = 0;            // The time that the last alarm was rung
int alarm_buzzer = 0;           // 1 = on, 0 = off
uint8_t temperature = 0;
float humidity = 0;
uint16_t light_intensity = 0;
float V_out = 0;
long current_time = 0;          // How long the system has been running for
long system_time = 0;
int DEBUG = 1;
// bool threadReady = false;       //using this to finish writing the first column at the start of the song, before the column is played

int hours, mins, secs;
long lastInterruptTime = 0;     // Used for button debounce
int RTC;                        // Holds the RTC instance

// ===== ===== ==== Function Definitions ===== ==== ==== //

int init_buttons(){

	pinMode(START_STOP_BUTTON, INPUT);
    pullUpDnControl(START_STOP_BUTTON, PUD_UP);

    pinMode(CHANGE_INTERVAL, INPUT);
    pullUpDnControl(CHANGE_INTERVAL, PUD_UP);

    pinMode(STOP_ALARM, INPUT);
    pullUpDnControl(STOP_ALARM, PUD_UP);

    pinMode(RESET_SYSTEM_TIME , INPUT);
    pullUpDnControl(RESET_SYSTEM_TIME , PUD_UP);

    // if (wiringPiISR(START_STOP_BUTTON, INT_EDGE_FALLING, &toggle_monitor) != 0){
    //     printf("registering isr for stop/start button failed.");
    // }
    //
    // if (wiringPiISR(CHANGE_INTERVAL, INT_EDGE_FALLING, &change_interval) != 0){
    //     printf("registering isr for inverval button failed.");
    // }
    //
    //  if (wiringPiISR(STOP_ALARM, INT_EDGE_FALLING, &turn_off_alarm) != 0){
    //     printf("registering isr for stop alarm button failed.");
    // }
    //
    // if (wiringPiISR(RESET_SYSTEM_TIME , INT_EDGE_FALLING, &reset_time) != 0){
    //     printf("registering isr for reset button failed.");
    // }

    return 1;
}

void toggle_monitor(){
    monitor = !monitor;
}

int init_ADC(){
    wiringPiSPISetup(0, 500000);
    return 1;
}

int init_DAC(){
    wiringPiSPISetup(1, 409600);
    return 1;
}

int init_RTC(){
    RTC = wiringPiI2CSetup(RTCADDR);
    return 1;
}

int write_DAC(){
    uint16_t temp = ((uint16_t)((V_out / 3.3) * 1024) & 1023);
    unsigned char data[2];

    data[0] = 0x30 | (temp >> 6);

    data[1] = 0x00 | (temp << 2);

    wiringPiSPIDataRW(1, data, 2);

    return 1;
}

int read_ADC(){
    for (int i = 0; i < 3; i++){
        unsigned char buffer[3];
        buffer[0] = 0x01;
        buffer[2] = 0x00;

        if (i == 0){
            buffer[1] = 0x80;
            wiringPiSPIDataRW(0, buffer, 3);
            uint16_t temp = get_ADC_value(buffer);
            temperature = ((3.3 * (temp/1024)) - 0.5)/0.01;
        }
        else if (i == 1){
            buffer[1] = 0x90;
            wiringPiSPIDataRW(0, buffer, 3);
            light_intensity = get_ADC_value(buffer);
        }
        else{
            buffer[1] = 0xA0;
            wiringPiSPIDataRW(0, buffer, 3);
            uint16_t temp = get_ADC_value(buffer);
            humidity = 3.3 * (humidity/1024)
        }

    }
    return 1;
}

uint16_t get_ADC_value(char* arr){
    uint16_t value = arr[1];

    value = (value << 8) | arr[2];

    return value;
}

int reset_time(){
    return 1;
}

int copy_arr(char* arr1, char* arr2){
    for (int i = 0; i < 24; i++){
        arr2[i] = arr1[i];
    }
    return 1;
}

void print_arr(char* arr){
    for (int i = 0; i < 24; i++){
        printf("%d - %c\n",i, arr[i] );
    }
}

int update_Blynk(){
    /* Tatenda, this is your section */
    return 1;
}

int get_Blynk_command(){
    /* Tatenda this is your section */
    return 1;
}

int change_interval() {
    //Simply cycles through the different values of interval values
    if (interval == 1){
        interval = 2;
    }
    else if (interval == 2){
        interval = 5;
    }
    else {
        interval = 1;
    }
    return 1;
}

long get_time() {
    /* Get time from RTC and from real world */

    return 0;
}

float calculate_Vout() {
    float ans = (light_intensity / 1023) * humidity;
    V_out = ans;
    return ans;
}

int turn_off_alarm(){
    alarm_buzzer = 0;
    return 1;
}

int sound_alarm() {
    // Check if good time to sound alarm //
    alarm_buzzer = 1;
    return 1;
}


// ===== ===== RTC FUNCTIONS ==== ==== //
int hFormat(int hours){
	/*formats to 12h*/
	if (hours >= 24){
		hours = 0;
	}
	else if (hours > 12){
		hours -= 12;
	}
	return (int)hours;
}

int hexCompensation(int units){
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}

int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}

int print_heading(){
    printf(" ------------------------------------------------------------------------\n");
    printf("| RTC Time | Sys Time | Humidity | Temperature | Light | DAC Out | Alarm |\n");
    printf("|----------|----------|----------|-------------|-------|---------|-------|\n");
    return 1;
}

int print_values(){
    if (!DEBUG){
        return 0;
    }


    //RTC Time Sys Timer Humidity Temp Light DAC out Alarm
    printf("| %8s | %8s |   %1.1f V  |     %2d C    | %5d |   %1.2f  |   %1c   |\n","test","test", humidity, temperature, light_intensity, V_out ,'*' );
    return 1;
}

int main(int argc, char const *argv[]) {
    /* code */
    //Set up wiring Pi
    wiringPiSetup();
    init_buttons();

    init_ADC();
    init_DAC();

    // // Thread setup
    // pthread_attr_t tattr;
    // pthread_t thread_id;
    // int newprio = 99;
    // sched_param param;
    //
    // pthread_attr_init (&tattr);
    // pthread_attr_getschedparam (&tattr, &param); /* safe to get existing scheduling param */
    // param.sched_priority = newprio; /* set the priority; others are unchanged */
    // pthread_attr_setschedparam (&tattr, &param); /* setting the new scheduling param */
    // pthread_create(&thread_id, &tattr, read_ADC, (void *)1); /* with new priority specified
    // print_heading();


    for (;;){
        write_DAC();
        read_ADC();
        print_values();
        delay(interval * 1000);
    }

    // --- ---- --- Debug Lines --- ---- ---//
    printf("Temp: %3.3f\nHumidity: %3.3f\nLight: %3.3f\nV_out: %3.3f\nAlarm?: %d\nTime: %s\nInterval: %d\n",temperature,humidity, light_intensity, V_out, alarm_buzzer, "Coming soon", interval );
    return 0;
}
