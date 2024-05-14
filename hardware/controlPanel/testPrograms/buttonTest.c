#include <stdio.h>
#include <pigpiod_if2.h>


#define BUTTON	10
#define NET_LED	22


void button_cb(int pi, unsigned user_gpio, unsigned edge, unint32_t tick);		// callback function (is called when the button state changes)

int main(){
    // init gpio client
    const int pi = pigpio_start(NULL, NULL);
    if(pi < 0){
        printf("Raspberry Pi not found");
        return;
    }

    // init gpios
	set_mode(pi, NET_LED, PI_OUTPUT);
	set_mode(pi, BUTTON, PI_INPUT);
	set_glitch_filter(pi, BUTTON, 20000);	// ingor all input changes for 20ms after the input state changes
	const int cb_id = callback(pi, BUTTON, EITHER_EDGE, button_cb);
    if(cb_id < 0){
        printf("init callback failed");
        return;
    }

    while(1){
		time_sleep(1);
    }

	return 0;
}

void button_cb(int pi, unsigned user_gpio, unsigned edge, unint32_t tick){
	if(edge == RISING_EDGE){
		gpio_write(pi, NET_LED, 1);
		printf("Button pressed\n");
	}
	else if(edge == FALLING_EDGE){
		gpio_write(pi, NET_LED, 0);
		printf("Button released\n");		
	}
}