#This is how the code will be divided up:
# Humphrey: Fix Vout in schematic, I2C: RTC + Alarm, SPI : ADC and DAC
# Tatenda: Buttons: Interrupts, Blynk linking

import RPi.GPIO as GPIO
import time

def init_SPI():
    print("SPI setup")

def init_I2C():
    print("I2C setup")

def sound_alarm():
    pass

def read_from_ADC():
    pass

def write_to_dac():
    pass

def setup():
    init_I2C()
    init_SPI()

def main():
    print("write your logic here")


# Only run the functions if
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        setup()
        while True:
            main()
            time.sleep(1)
    except KeyboardInterrupt:
        print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
    except Exception as e:
        GPIO.cleanup()
        print("Some other error occurred")
        print(e.message)
