#include <stdio.h>
#include <pigpiod_if2.h>

#define ENABLE	24
#define PHASE	18
#define NFAULT	23

int main(){
    // init gpio client
    const int pi = pigpio_start(NULL, NULL);
    if(pi < 0){
        printf("Raspberry Pi not found");
        return;
    }

    // init gpios
    set_mode(pi, ENABLE, PI_OUTPUT);
    set_mode(pi, PHASE, PI_OUTPUT);
    set_mode(pi, NFAULT, PI_INPUT);

    // turn on the motor
    gpio_write(pi, PHASE, 1);
    gpio_write(pi, ENABLE, 1);

    while(gpio_read(pi, NFAULT))	// wait until the torque gets to high
    printf("nFault turned low");

	return 0;
}
