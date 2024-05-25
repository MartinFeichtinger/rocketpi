#include <stdio.h>
#include <stdbool.h>
#include <pigpiod_if2.h>

// powerBoard
#define ENABLE		24
#define PHASE		18
#define NFAULT		23

// controlPanel
#define GPS_LED		17
#define BOOT_LED	27
#define NET_LED		22
#define BUTTON		10


void nfault_cb(int pi, unsigned user_gpio, unsigned edge, uint32_t tick);
void button_cb(int pi, unsigned user_gpio, unsigned edge, uint32_t tick);

bool shaking=false;

int main(){
	////////////////////////// INIT GPIO SETTINGS //////////////////////////
	// init gpio client
	const int pi = pigpio_start(NULL, NULL);
	if(pi < 0){
		printf("Raspberry Pi not found");
		return -1;
	}

	// init gpios
	set_mode(pi, ENABLE, PI_OUTPUT);
	set_mode(pi, PHASE, PI_OUTPUT);
	set_mode(pi, NFAULT, PI_INPUT);

	set_mode(pi, GPS_LED, PI_OUTPUT);
	set_mode(pi, BOOT_LED, PI_OUTPUT);
	set_mode(pi, NET_LED, PI_OUTPUT);
	set_mode(pi, BUTTON, PI_INPUT);

	// init button spefic gpio settings
	set_glitch_filter(pi, BUTTON, 20000);	// ingor all input changes for 20ms after the input state changes
	const int cb_id = callback(pi, BUTTON, EITHER_EDGE, button_cb);
	if(cb_id < 0){
		printf("init callback failed");
		return -1;
	}

	// cofigurate the pwm
	int pwm_frequency = 8000;
	int real_pwm_frequency = set_PWM_frequency(pi, ENABLE, pwm_frequency);
	printf("pwm_frequency: set=%d; real=%d\n", pwm_frequency, real_pwm_frequency);

	int pwm_range = 25;
	int real_pwm_range = set_PWM_range(pi, ENABLE, pwm_range);	// => 1 = 1%
	printf("pwm_range: set=%d; real=%d\n", pwm_range, real_pwm_range);

	// init NFAULT callback function
	const int nfault_cb_id = callback(pi, NFAULT, EITHER_EDGE, nfault_cb);
	if(nfault_cb_id < 0){
		printf("init callback failed");
		return -1;
	}

	////////////////////////// START OF THE REAL SHIT //////////////////////////
	gpio_write(pi, BOOT_LED, 0);

	while(1){
		if(shaking){
			set_PWM_dutycycle(pi, ENABLE, 10);
			gpio_write(pi, PHASE, 0);
			time_sleep(0.07);
			gpio_write(pi, PHASE, 1);
			time_sleep(0.07);
		}
		else{
			set_PWM_dutycycle(pi, ENABLE, 0);
		}
	}
}

void nfault_cb(int pi, unsigned user_gpio, unsigned edge, uint32_t tick){
	if(edge == FALLING_EDGE){
		printf("nFault turned LOW\n");
		gpio_write(pi, NET_LED, 1);
	}
	else if(edge == RISING_EDGE){
		printf("nFault turned HIGH\n");
		gpio_write(pi, NET_LED, 0);
	}
	else{
		printf("some wierd timeout occured\n");
	}
}

void button_cb(int pi, unsigned user_gpio, unsigned edge, uint32_t tick){
	if(edge == RISING_EDGE){
		gpio_write(pi, GPS_LED, 1);
		shaking=true;
		printf("Button pressed\n");
	}
	else if(edge == FALLING_EDGE){
		gpio_write(pi, GPS_LED, 0);
		shaking=false;
		printf("Button released\n");
	}
}
