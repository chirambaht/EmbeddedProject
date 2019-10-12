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
    return 1;
}

int init_ADC(){
    digitalWrite (CS0, HIGH);
    ADC = wiringPiSPISetup(0, SPI_SPEED);
    return 1;
}

int init_DAC(){
    digitalWrite (CS1, HIGH);
    return 1;
}

int init_RTC(){
    RTC = wiringPiI2CSetup(RTCADDR);
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

int read_ADC(){
    digitalWrite (CS0, LOW);
    // Cycle through each ADC channel and read the value. Not converted!.
    for (int adcnum = 0; adcnum < 3; adcnum++){
        unsigned int commandout = 0;
        unsigned int adcout = 0;

        commandout = adcnum & 0x3;  // only 0-7
        commandout |= 0x18;     // start bit + single-ended bit

        unsigned char spibuf[3];

        spibuf[0] = commandout;
        spibuf[1] = 0;
        spibuf[2] = 0;

        wiringPiSPIDataRW(1, spibuf, 3);
        for (int i = 0; i < 3; i++){
            printf("%d - %d\n",i ,spibuf[i]);
        }

        adcout = ((spibuf[1] << 8) | (spibuf[2])) >> 4;

        if (adcnum == 0){
            temperature = adcout;
            printf("Temp: %d\n", adcout);
        }
        else if (adcnum == 1){
            humidity = adcout;
            printf("Humidity: %d\n", adcout);
        }
        else{
            light_intensity = adcout;
            printf("Light: %d\n", adcout);
        }

    }
    digitalWrite (CS0, HIGH);
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

int main(int argc, char const *argv[]) {
    /* code */
    printf("Running the puppy\n");
    init_ADC();
    printf("ADC initialised!\n");
    read_ADC();
    printf("V_out: %f\n", calculate_Vout());

    if (V_out >= 2.65 || V_out <= 0.65){
        sound_alarm();
    }
    return 0;
}
