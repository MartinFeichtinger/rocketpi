#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pigpiod_if2.h>
#include <signal.h>

// file paths
#define OUTPUT_FOLDER	"/home/pi/rocketpi/measurements/"

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
int file_handle;

enum State {OPEN, SHAKING, CLOSING, LOADED, ARMED, UNARMED, OPENING, FLYING, FLYING_OPEN};
enum State state = OPEN;

uint32_t last_rising_tick;
bool button_pressed;

uint32_t timer_start;

bool saveMeasurements = false;
uint32_t timestamp;
int flyingState=0;
int16_t accX, accY, accZ, gyrX, gyrY, gyrZ, tVal;
int16_t acc;
double temp = 0.0;

// function prototypes
bool init();
bool generateNewFile();
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
						generateNewFile();
						saveMeasurements=true;
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
						saveMeasurements=false;
						file_close(pi, file_handle);
						printf("rocketpi unarmed\n\n");
						state = UNARMED;
					}
				}

				// if accelerations is detected
				if(acc >= 2*9810){
					timer_start = get_current_tick(pi);
					flyingState=1;
					state = FLYING;
				}
				break;

			case UNARMED:
				// wait for button press to switch to OPENING
				break;

			case OPENING:
				gpio_write(pi, PHASE, 1);
				set_PWM_dutycycle(pi, ENABLE, 10);
				time_sleep(1);
				set_PWM_dutycycle(pi, ENABLE, 0);

				gpio_write(pi, LOADED_LED, 0);
				state = OPEN;
				break;

			case FLYING:
				if(get_current_tick(pi) >= timer_start + 2000*1000){
					timer_start = get_current_tick(pi);
					flyingState=2;
					state = FLYING_OPEN;
				}
				break;

			case FLYING_OPEN:
				if(get_current_tick(pi) < timer_start + 1000*1000){
	                                // opening parachute
	                                gpio_write(pi, PHASE, 1);
	                                set_PWM_dutycycle(pi, ENABLE, 10);
				}
				else{
                                        set_PWM_dutycycle(pi, ENABLE, 0);
                                        gpio_write(pi, LOADED_LED, 0);
                                        flyingState=3;
				}

				if(get_current_tick(pi) >= timer_start + 15000*1000){
					gpio_write(pi, ARMED_LED, 0);
					saveMeasurements=false;
					flyingState=0;
					file_close(pi, file_handle);
					printf("Parachute opened\n\n");
					state = OPEN;
				}
				break;
		}

		if(saveMeasurements){
		timestamp=get_current_tick(pi)/1000;
		char str[300];
		readMPU6050(pi, i2c_handle);
		sprintf(str, "%d	%d	%d	%d	%d	%d	%d	%d	%d	%.2f\n", timestamp, flyingState, acc, accX, accY, accZ, gyrX, gyrY, gyrZ, temp);
		printf(str);

		int i=0;
		while(str[i] != '\0'){
			i++;
		}

		if(file_write(pi, file_handle, str, i) == 0){
			//printf("Stored to file sucessfully\n");
		}
		else{
			printf("Unable to save to file\n");
		}
	}

	time_sleep(0.05);
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

bool generateNewFile(){
	char searchString[] = OUTPUT_FOLDER "mpu6050-*.txt";
	char files[1000];
	int c = file_list(pi, searchString, files, sizeof(files));

	char filename[20];
	if(c >= 0){
		files[c]=0;

		int numberOfFiles=0;
		for(int i=0; i<c; i++){
			if(files[i] == '\n'){
				numberOfFiles++;
			}
		}
		sprintf(filename, "mpu6050-%.2d.txt", numberOfFiles);
	}
	else{
		strcpy(filename, "mpu6050-00.txt");
	}

	char outputFile[] = OUTPUT_FOLDER;
	strcat(outputFile, filename);
	printf("output file: %s\n", outputFile);

	char command[100];
	sprintf(command, "touch %s", outputFile);
	system(command);

	file_handle = file_open(pi, outputFile, PI_FILE_WRITE);
	if(file_handle >= 0){
		printf("File opend succesfully\n\n");
		char init_headline[] = {"time	state	acc	accX	accY	accZ	gyrX	gyrY	gyrZ	temp\n"};

		if(file_write(pi, file_handle, init_headline, sizeof(init_headline)-1) == 0){
			printf(init_headline);
		}
		else{
			printf("Unable to save to file\n");
			return false;
		}
	}
	else{
		printf(pigpio_error(file_handle));
		return false;
	}

	return true;
}

void readMPU6050(){
	// read accelerometer values
	i2c_write_byte_data(pi, i2c_handle, 0x3B, 0x00);
	accX = i2c_read_byte_data(pi, i2c_handle, 0x3B) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x3C);
	accY = i2c_read_byte_data(pi, i2c_handle, 0x3D) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x3E);
	accZ = i2c_read_byte_data(pi, i2c_handle, 0x3F) << 8 | i2c_read_byte_data(pi, i2c_handle, 0x40);
	acc = sqrt(accX*accX + accY*accY + accZ*accZ);
	
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
		if(state == SHAKING){
			state = CLOSING;
		}
	}
}

void sigintlHandler(int signal){
	printf("\n\nProgramm interupted (ctrl+c)\n");
	printf("i2c_handler stopped\n");
	printf("file closed\n");
	i2c_close(pi, i2c_handle);
	file_close(pi, file_handle);
	exit(0);
}
