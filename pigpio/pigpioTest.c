#include <stdio.h>
#include <pigpiod_if2.h>


#define LED_PIN	17
#define T_ON	1
#define T_OFF	1


int main(){
    // init gpio client
    const int pi = pigpio_start(NULL, NULL);
    if(pi < 0){
        printf("Raspberry Pi not found");
        return;
    }

    // init gpios
    set_mode(pi, LED_PIN, PI_OUTPUT);

    while(1){
        gpio_write(pi, LED_PIN, 1);
        time_sleep(T_ON);
        gpio_write(pi, LED_PIN, 0);
        time_sleep(T_OFF);
    }
}
