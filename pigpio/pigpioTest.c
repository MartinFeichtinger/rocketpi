#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>     // to use usleep()


#define LED_PIN 17
#define T_ON    1000
#define T_OFF   1000

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
        usleep(T_ON*1000);
        gpio_write(pi, LED_PIN, 0);
        usleep(T_OFF*1000);
    }
}