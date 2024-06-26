#include <stdio.h>
#include <pigpiod_if2.h>

#define ENABLE	24
#define PHASE	18
#define NFAULT	23


void nfault_cb(int pi, unsigned user_gpio, unsigned edge, uint32_t tick);		// callback function (is called when the nFault state changes)

int main(int argc, char* argv[]){
	int dir;
	if(argc < 2){
		return -1;
	}
	if(*argv[1] == 'o'){
		dir=1;
	}
	else if(*argv[1] == 'c'){
		dir=0;
	}
	else{
		return -1;
	}
/	// init gpio client
	const int pi = pigpio_start(NULL, NULL);
	if(pi < 0){
		printf("Raspberry Pi not found");
		return 0;
	}

	// init gpios
	set_mode(pi, ENABLE, PI_OUTPUT);
	set_mode(pi, PHASE, PI_OUTPUT);
	set_mode(pi, NFAULT, PI_INPUT);

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
		return 0;
	}

	// turn on the motor
 	gpio_write(pi, PHASE, dir);
	set_PWM_dutycycle(pi, ENABLE, 10);

	// wait until the torque gets to high
	// stopping the motor is handled with a special callback function
	while(1){
		time_sleep(1);
		break;
	}

	set_PWM_dutycycle(pi, ENABLE, 0);
	callback_cancel(nfault_cb_id);
	//gpioTerminate();
	return 0;
}

void nfault_cb(int pi, unsigned user_gpio, unsigned edge, uint32_t tick){
	if(edge == FALLING_EDGE){
		printf("nFault turned LOW\n");
		//set_PWM_dutycycle(pi, ENABLE, 0);		// stop the motor
	}
	else if(edge == RISING_EDGE){
		printf("nFault turned HIGH\n");
	}
	else{
		printf("some wierd timeout occured\n");
	}
}
