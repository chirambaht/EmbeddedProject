#include "main.h"
#include "CurrentTime.h"

// ===== ===== ==== Variable Definitions ===== ==== ==== //
long last_press = 0;            //Used for debouncing.
int monitor = 0;                // 1 = on, 0 = off
int interval = 1;               // How often values will be logged
// long last_alarm = 0;            // The time that the last alarm was rung
tyme current, last_alarm, actual, start_time;
long last_alarm_press = -9999999;
int alarm_buzzer = 0;           // 1 = on, 0 = off
uint8_t temperature = 0;
float humidity = 0;
uint16_t light_intensity = 0;
float V_out = 0;
long current_time = 0;          // How long the system has been running for
long system_time = 0;
int DEBUG = 1;                  // 1 is Debug mode
// bool threadReady = false;       //using this to finish writing the first column at the start of the song, before the column is played

int hours, mins, secs;
int HH,MM,SS;
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

    if (wiringPiISR(START_STOP_BUTTON, INT_EDGE_FALLING, &toggle_monitor_isr) != 0){
        printf("registering isr for stop/start button failed.");
    }

    if (wiringPiISR(CHANGE_INTERVAL, INT_EDGE_FALLING, &toggle_interval_isr) != 0){
        printf("registering isr for inverval button failed.");
    }

     if (wiringPiISR(STOP_ALARM, INT_EDGE_FALLING, &turn_off_alarm_isr) != 0){
        printf("registering isr for stop alarm button failed.");
    }

    if (wiringPiISR(RESET_SYSTEM_TIME , INT_EDGE_FALLING, &reset_time_isr) != 0){
        printf("registering isr for reset button failed.");
    }

    return 1;
}

void toggle_monitor(){
    if (!monitor){
        start_time = current;
    }
    monitor = !monitor;
}

void toggle_monitor_isr(){
    long interruptTime = millis();
    if (interruptTime - last_press > DEBOUNCE_TIME){
        toggle_monitor();
    }
    last_press = interruptTime;
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
    // set_time(now());
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
    uint8_t buffer[3];
    for (int i = 0; i < 3; i++){

        buffer[0] = 0x01;
        buffer[2] = 0x00;

        if (i == 0){
            buffer[1] = 0x80;
            wiringPiSPIDataRW(0, buffer, 3);
            uint16_t temp = (buffer[1] << 8) | buffer[2];
            temperature = ((3.3 * (temp/1024.0f)) - 0.5)/0.01;
        }
        else if (i == 1){
            buffer[1] = 0x90;
            wiringPiSPIDataRW(0, buffer, 3);
            light_intensity = (buffer[1] << 8) | buffer[2];
        }
        else{
            buffer[1] = 0xA0;
            wiringPiSPIDataRW(0, buffer, 3);
            uint16_t temp = (buffer[1] << 8) | buffer[2];
            humidity = (3.3 * (temp/1024.0f));
        }

    }
    return 1;
}

void reset_time(){
    monitor = 0;

    actual.hours = 0;
    actual.seconds = 0;
    actual.minutes = 0;

    last_alarm_press = -9999999;

    system("clear");

    print_heading();
}

void reset_time_isr(){
    long interruptTime = millis();
    if (interruptTime - last_press > DEBOUNCE_TIME){
        reset_time();
    }
    last_press = interruptTime;
}

int update_Blynk(){
    /* Tatenda, this is your section */
    return 1;
}

int get_Blynk_command(){
    /* Tatenda this is your section */
    return 1;
}

void toggle_interval() {
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
}

void toggle_interval_isr() {
    //Simply cycles through the different values of interval values
    long interruptTime = millis();
    if (interruptTime - last_press > DEBOUNCE_TIME){
        toggle_interval();
    }
    last_press = interruptTime;
}

tyme now(){
    tyme temp;
    temp.seconds = wiringPiI2CReadReg8(RTC, SEC);
    temp.minutes = wiringPiI2CReadReg8(RTC, MIN);
    temp.hours = wiringPiI2CReadReg8(RTC, HOUR);
    return temp;
}

long get_time() {
    if (monitor){
        current.seconds += interval;
    }

    if (current.seconds == 60){
        current.minutes += 1;
        current.seconds = 0;
    }
    if (current.minutes == 60){
        current.hours += 1;
        current.minutes = 0;
    }
    if (current.hours == 24){
        current.hours = 0;
    }


    return 0;
}

void set_time(tyme t){
    // wiringPiI2CWriteReg8(RTC,SEC,(t.seconds | 0b1 << 7));
    wiringPiI2CWriteReg8(RTC,SEC, 0b10000000+t.seconds);
    wiringPiI2CWriteReg8(RTC,MIN,t.minutes);
    wiringPiI2CWriteReg8(RTC,HOUR,t.hours);
}

float calculate_Vout() {
    float ans = (light_intensity / 1023.0f) * humidity;
    V_out = ans;
    if (V_out > 2.65 || V_out < 0.65){
        sound_alarm();
    }
    return ans;
}

void turn_off_alarm(){
    alarm_buzzer = 0;
}

void turn_off_alarm_isr(){
    long interruptTime = millis();
    if (interruptTime - last_press > DEBOUNCE_TIME){
        // tyme t = time_difference(current, actual);
        // printf("%2d:%2d:%2d\n",t.hours,t.minutes, t.seconds );
        turn_off_alarm();
    }
    last_press = interruptTime;
}

tyme time_difference(tyme a, tyme b){
    tyme temp = {0,0,0};

    int sum_a = a.seconds + (60 * a.minutes) + (60 * 60 * a.hours);
    int sum_b = b.seconds + (60 * b.minutes) + (60 * 60 * b.hours);
    int sum = abs(sum_a - sum_b);

    while (sum >= 60){
        if (sum >= 60){
            temp.minutes += 1;
            sum -= 60;
        }
        if (temp.minutes >= 60){
            temp.hours += 1;
            temp.minutes -= 60;
        }
    }

    temp.seconds = sum;

    return temp;
}

int sound_alarm() {
    // Check if good time to sound alarm //
    if (millis() - last_alarm_press > ALARM_WAIT){
        alarm_buzzer = 1;
        last_alarm_press = millis();
    }

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
    char a = ' ';

    if (alarm_buzzer){
        a = '*';
    }
    tyme runtime = time_difference(current, start_time);

    printf("| %2d:%2d:%2d | %2d:%2d:%2d |   %1.1f V  |     %2d C    | %5d |   %1.2f  |   %1c   |\n",current.hours,current.minutes,current.seconds,runtime.hours,runtime.minutes,runtime.seconds, humidity, temperature, light_intensity, V_out ,a );
    return 1;
}

// void get_current_time(){
//     HH = getHours();
//     MM = getMins();
//     SS = getSecs();
//
//     HH = hFormat(HH);
//     HH = decCompensation(HH);
//     // wiringPiI2CWriteReg8(RTC, HOUR, HH);
//
//     MM = decCompensation(MM);
//     // wiringPiI2CWriteReg8(RTC, MIN, MM);
//
//     SS = decCompensation(SS);
//     // wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);
// }

int main(int argc, char const *argv[]) {
    /* code */
    //Set up wiring Pi
    wiringPiSetup();
    init_buttons();

    init_ADC();
    init_DAC();

    init_RTC();

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
    print_heading();
    tyme temp = {0x12 + TIMEZONE,0x30,0x53};
    current.hours = hexCompensation(temp.hours);
    current.minutes = hexCompensation(temp.minutes);
    current.seconds = hexCompensation(temp.seconds);
    for (;;){
        if (monitor){
            read_ADC();
            calculate_Vout();
            write_DAC();
            print_values();
        }
        get_time();
        delay(interval * 1000);
    }


    return 0;
}

void wait(){

}
