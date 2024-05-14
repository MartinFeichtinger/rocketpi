#include <stdio.h>
#include <pigpiod_if2.h>


#define GPS_LED		17
#define BOOT_LED	27
#define NET_LED		22


void blink(int gpio, double period, int repetition);

int main(){
	// init gpio client
	const int pi = pigpio_start(NULL, NULL);
	if(pi < 0){
		printf("Raspberry Pi not found");
		return;
	}

	// init gpios
	set_mode(pi, GPS_LED, PI_OUTPUT);
	set_mode(pi, BOOT_LED, PI_OUTPUT);
	set_mode(pi, NET_LED, PI_OUTPUT);


	// let the boot led blink for 2 seconds
	// (The boot LED is controlled by an lm555. During booting, the boot LED flashes and after booting is complete,
	// the BOOT_LED pin must be set to low so that the LED remains lit)
	gpio_write(pi, BOOT_LED, 1);
	time_sleep(2);
	gpio_write(pi, BOOT_LED, 0);

	// let the GPS_LED blink for 2 seconds
	blink(GPS_LED, 1, 2);

	// let the NET_LED blink for 2 seconds
	blink(NET_LED, 1, 2);

	printf("finished ledTest\n");
	return 0;
}

void blink(int gpio, double period, int repetition){
	for(int i=0; i<repetition; i++){
		gpio_write(pi, gpio, 1);
		time_sleep(period/2)
		gpio_write(pi, gpio, 0);
		time_sleep(period/2);		
	}
}