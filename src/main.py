#This is how the code will be divided up:
# Humphrey: Fix Vout in schematic, I2C: RTC + Alarm, SPI : ADC and DAC
# Tatenda: Buttons: Interrupts, Blynk linking

import RPi.GPIO as GPIO
import time
import spidev
# from smbus2 import SMBus
import smbus


RTC = 0
RTC_address = 0

spi = spidev.SpiDev() # create spi object
spi.open(0, 1) # open spi port 0, device (CS) 1

try:
    RTC = smbus.SMBus(1)
    RTC_address = 0x6f
except:
    print("Failed to setup I2C")

try:
    spi = spidev.SpiDev() # create spi object
    spi.open(0, 1) # open spi port 0, device (CS) 1
except:
    print("Failed to setup SPI")

ADC = 1
DAC = 3

def sound_alarm():
    pass

def read_from_ADC(channel):
    pass

def get_time():
    b = bus.read_byte_data(RTC_address, 0) #Read from adress and offset 0
    return b

def write_to_dac():
    pass

def setup():
    pass

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
