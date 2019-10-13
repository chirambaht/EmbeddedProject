#include "main.h"

// ===== ===== ==== Variable Definitions ===== ==== ==== //
int monitor = 0;                // 1 = on, 0 = off
int interval = 1;               // How often values will be logged
long last_alarm = 0;            // The time that the last alarm was rung
int alarm_buzzer = 0;           // 1 = on, 0 = off
float temperature = 0;
float humidity = 0;
float light_intensity = 0;
float V_out = 0;
long current_time = 0;          // How long the system has been running for
long system_time = 0;


int ADC;
int hours, mins, secs;
long lastInterruptTime = 0;     // Used for button debounce
int RTC;                        // Holds the RTC instance

// ===== ===== ==== Function Definitions ===== ==== ==== //

int init_buttons(){
    /* Tatenda, this is one of your sections. Before doing this
    please go look at the main.h file for button names. For all
    your functions, please return 0 if there is an error and
    comment your code where needed.*/
     //Set up wiring Pi
    wiringPiSetup();
    
    //setting up the buttons
    
	pinMode(START_STOP_BUTTON, INPUT);
    pullUpDnControl(START_STOP_BUTTON, PUD_UP);
    
    pinMode(CHANGE_INTERVAL, INPUT);
    pullUpDnControl(CHANGE_INTERVAL, PUD_UP);

    pinMode(STOP_ALARM, INPUT);
    pullUpDnControl(STOP_ALARM, PUD_UP);

    pinMode(RESET_SYSTEM_TIME , INPUT);
    pullUpDnControl(RESET_SYSTEM_TIME , PUD_UP);
    
    //setting up the SPI interface
    
    if (wiringPiISR(START_STOP_BUTTON, INT_EDGE_FALLING, &play_pause_isr) != 0){
        printf("registering isr for play button failed.");
    }

    if (wiringPiISR(CHANGE_INTERVAL, INT_EDGE_FALLING, &stop_isr) != 0){
        printf("registering isr for stop button failed.");
    }

     if (wiringPiISR(STOP_ALARM, INT_EDGE_FALLING, &play_pause_isr) != 0){
        printf("registering isr for play button failed.");
    }

    if (wiringPiISR(RESET_SYSTEM_TIME , INT_EDGE_FALLING, &stop_isr) != 0){
        printf("registering isr for stop button failed.");
    }

    return 1;
}

int init_ADC(){
    wiringPiSPISetup (0, 409600) ;
    return 1;
}

int init_DAC(){
    //Speed = 409600
    wiringPiSPISetup (1, 409600);
    return 1;
}

int init_RTC(){
    RTC = wiringPiI2CSetup(RTCADDR);
    return 1;
}

int write_DAC(){
    V_out = 2.1;
    int temp = (V_out / 3.3) * 4096;
    unsigned char data[2];

    data[0] = 0xF0;
    data[0] = data[0] | (temp << 2);
    wiringPiSPIDataRW(1, data, 1);

    return 1;
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
    printf("__________________________________________________________________________\n");
    printf("| RTC Time | Sys Time | Humidity | Temperature | Light | DAC Out | Alarm |\n");
    return 1;
}

int print_values(){
    //RTC Time Sys Timer Humidity Temp Light DAC out Alarm
    printf("| %8s | %8s |   %1.1f V  |     %2d C    | %5d |   %1.2f  |   %1c   |\n","test","test", 2.3, 25, 1000, 0.23 ,'*' );
    return 1;
}

int main(int argc, char const *argv[]) {
    /* code */
    init_ADC();
    init_DAC();

    write_DAC();

    print_heading();
    for (;;){
        print_values();
        write_DAC();
        delay(interval * 1000);
    }

    // --- ---- --- Debug Lines --- ---- ---//
    printf("Temp: %3.3f\nHumidity: %3.3f\nLight: %3.3f\nV_out: %3.3f\nAlarm?: %d\nTime: %s\nInterval: %d\n",temperature,humidity, light_intensity, V_out, alarm_buzzer, "Coming soon", interval );
    return 0;
}
