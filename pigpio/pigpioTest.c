#include <stdio.h>
#include <pigpiod_if2.h>

#define LED_PIN 17
#define T_ON    1000
#define T_OFF   1000

int main(){
    // init gpio client
    const int pi = pigpio_start();
    if(pi < 0){
        printf("Raspberry Pi not found");
        return;
    }

    // init gpios
    set_mode(pi, LED_PIN, PI_OUTPUT);

    while(true){
        gpio_write(pi, LED_PIN, 1);
        delay(T_ON);
        gpio_write(pi, LED_PIN, 0);
        delay(T_OFF);
    }
}