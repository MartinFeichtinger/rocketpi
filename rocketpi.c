#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pigpiod_if2.h>


// powerBoard
#define ENABLE			24
#define PHASE			18

// controlPanel
#define ARMED_LED		22		// NET_LED on the schematic
#define LOADED_LED		17		// GPS_LED on the schematic
#define BUTTON			10

// MPU6050
#define MPU6050_ADRESS	0x68

// global variables
int pi;
int i2c_handle;

enum State {OPEN, SHAKING, CLOSING, LOADED, ARMED, UNARMED, FLYING, OPENING};
enum State state = OPEN;

uint32_t last_rising_tick;
bool button_pressed;

int16_t accX, accY, accZ, gyrX, gyrY, gyrZ, tVal;
double temp = 0.0;

// function prototypes
bool init();
void sigintlHandler(int signal);
void readMPU6050();
void button_cb(int pi, unsigned user_gpio, unsigned edge, uint32_t tick);


int main(){
	signal(SIGINT, sigintlHandler); // this handler is called when ctrl+c is pressed

	if(init() == false){
		return -1;
	}

	while(true){
		switch(state){
			case OPEN:
				// wait for button press
				break;

			case SHAKING:
				set_PWM_dutycycle(pi, ENABLE, 10);
				gpio_write(pi, PHASE, 0);
				time_sleep(0.07);
				gpio_write(pi, PHASE, 1);
				time_sleep(0.07);
				// do this until the button get released
				break;

			case CLOSING:
				gpio_write(pi, PHASE, 0);
				set_PWM_dutycycle(pi, ENABLE, 10);
				time_sleep(1);
				set_PWM_dutycycle(pi, ENABLE, 0);

				gpio_write(pi, LOADED_LED, 1);
				state = LOADED;
				break;

			case LOADED:
				if(button_pressed){
					uint32_t duration = (get_current_tick(pi) - last_rising_tick)/1000; 	// milliseconds
					if(duration >= 2000){
						button_pressed = false;

						gpio_write(pi, ARMED_LED, 1);
						state = ARMED;
					}
				}
				break;

			case ARMED:
				if(button_pressed){
					uint32_t duration = (get_current_tick(pi) - last_rising_tick)/1000; 	// milliseconds
					if(duration >= 2000){
						button_pressed = false;

						gpio_write(pi, ARMED_LED, 0);
						state = UNARMED;
					}
				}
				// if acc >= bla bla then state=FLYING;
				// already store data to file
				break;

			case UNARMED:
				// wait for button press
				break;

			case FLYING:
				// Code for FLYING state
				break;

			case OPENING:
				gpio_write(pi, PHASE, 1);
				set_PWM_dutycycle(pi, ENABLE, 10);
				time_sleep(1);
				set_PWM_dutycycle(pi, ENABLE, 0);

				gpio_write(pi, LOADED_LED, 0);
				state = OPEN;
				break;
		}
	}
}

bool init(){
	// init gpio client
	pi = pigpio_start(NULL, NULL);
	if(pi < 0){
		printf("Raspberry Pi not found");
		return -1;
	}

	// init gpios
	set_mode(pi, ENABLE, PI_OUTPUT);
	set_mode(pi, PHASE, PI_OUTPUT);

	set_mode(pi, ARMED_LED, PI_OUTPUT);
	set_mode(pi, LOADED_LED, PI_OUTPUT);
	set_mode(pi, BUTTON, PI_INPUT);

	gpio_write(pi, ARMED_LED, 0);
	gpio_write(pi, LOADED_LED, 0);

	// cofigurate the pwm
	int pwm_frequency = 8000;
	int real_pwm_frequency = set_PWM_frequency(pi, ENABLE, pwm_frequency);
	printf("pwm_frequency: set=%d; real=%d\n", pwm_frequency, real_pwm_frequency);

	int pwm_range = 25;
	int real_pwm_range = set_PWM_range(pi, ENABLE, pwm_range);
	printf("pwm_range: set=%d; real=%d\n", pwm_range, real_pwm_range);

	// init button spefic gpio settings
	set_glitch_filter(pi, BUTTON, 20000);	// ingor all input changes for 20ms after the input state changes
	const int cb_id = callback(pi, BUTTON, EITHER_EDGE, button_cb);
	if(cb_id < 0){
		printf("init callback failed");
		return -1;
	}
	
	// init i2c
	i2c_handle = i2c_open(pi, 1, MPU6050_ADRESS, 0);
	if(i2c_handle < 0){
		printf("i2c device not found\n");
		return false;
	}

	// init mpu6050
	i2c_write_byte_data(pi, i2c_handle, 0x6B, 0x00);	// wake up mpu6050
	i2c_write_byte_data(pi, i2c_handle, 0x1B, 0x08);	// set full scale range of gyroscope to +-500°/s
	i2c_write_byte_data(pi, i2c_handle, 0x1C, 0x08);	// set full scale range of accelerometer to +-4g
	//i2c_write_byte_data(pi, i2c_handle, 0x19, 0x07);	// set sample rate to 1kHz
	i2c_write_byte_data(pi, i2c_handle, 0x1A, 0x00);	// set digital low pass filter to 260Hz
	i2c_write_byte_data(pi, i2c_handle, 0x6C, 0x00);	// disable sleep mode
	
	return true;
}

void readMPU6050(){
	// read accelerometer values
	i2c_write_byte_data(pi, i2c_handle, 0x3B, 0x00);
	accX = i2c_read_byte_data(pi, i2c_handle, 0x3B) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x3C);
	accY = i2c_read_byte_data(pi, i2c_handle, 0x3D) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x3E);
	accZ = i2c_read_byte_data(pi, i2c_handle, 0x3F) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x40);
	
	// read gyroscope values
	gyrX = i2c_read_byte_data(pi, i2c_handle, 0x43) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x44);
	gyrY = i2c_read_byte_data(pi, i2c_handle, 0x45) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x46);
	gyrZ = i2c_read_byte_data(pi, i2c_handle, 0x47) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x48);
	
	// read temperature value
	tVal = i2c_read_byte_data(pi, i2c_handle, 0x41) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x42);
	temp = (double)tVal / 340.0 + 36.53;
}

void button_cb(int pi, unsigned user_gpio, unsigned edge, uint32_t tick){
	if(edge == RISING_EDGE){
		printf("button pressed\n");

		if(state == OPEN){
			state = SHAKING;
		}
		else if(state == LOADED || state == ARMED){
			last_rising_tick = tick;
			button_pressed = true;
		}
		else if(state == UNARMED){
			state = OPENING;
		}
	}
	else if(edge == FALLING_EDGE){
		printf("button released\n");

		if(state == SHAKING){
			state = CLOSING;
		}
	}
}

void sigintlHandler(int signal){
	printf("signal: %d; i2c_handler stopped\n", signal);
	i2c_close(pi, i2c_handle);
	exit(0);
}