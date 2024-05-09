#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>

#define ENABLE	24
#define PHASE	18
#define nFAULT	23

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
    set_mode(pi, nFAULT, PI_INPUT);

    // turn on the motor
    gpio_write(pi, PHASE, 1);
    gpio_write(pi, ENABLE, 1);

    while(gpio_read(pi, nFAULT))	// wait until the torque gets to high
    printf("nFault turned low");

	return 0;
}
