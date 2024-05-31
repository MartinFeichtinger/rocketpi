#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pigpiod_if2.h>
#include <signal.h>

#define MPU6050_ADRESS	0x68
#define OUTPUT_FILE		"/home/pi/rocketpi/testPrograms/testFiles/mpu6050.txt"

int16_t accX, accY, accZ, gyrX, gyrY, gyrZ, tVal;
double temp = 0.0;
int pi;
int i2c_handle;
int file_handle;

// funciton prototypes
bool init();
void readMPU6050();
void sigintlHandler(int signal);

int main(){
	signal(SIGINT, sigintlHandler); // this handler is called when ctrl+c is pressed

	if(init() == false){
		return -1;
	}

	while(true){
		readMPU6050(pi, i2c_handle);
		printf("accX: %d;\taccY: %d;\taccZ: %d;\tgyrX: %d;\tgyrY: %d;\tgyrZ: %d;\ttemp: %f\n", accX, accY, accZ, gyrX, gyrY, gyrZ, temp);
		time_sleep(0.5);
	}
}


bool init(){
	// init pigpio
	pi = pigpio_start(NULL, NULL);
	if(pi < 0){
		printf("Raspberry Pi not found\n");
		return false;
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

	// open storage file
	file_handle = file_open(pi, OUTPUT_FILE, PI_FILE_WRITE);
	if(file_handle >= 0){
		printf("File opend succefully\n");
		char init_headline[] = {"Some random test headline, bla bla bla ...\n\n"};

		if(file_write(pi, file_handle, init_headline, sizeof(init_headline)) == 0){
			printf("Stored to file sucessfully\n");
		}
		else{
			printf("Unable to save to file\n");
			return -1;
		}
	}
	else{
		printf("Can not open file. Error %d\n", file_handle);
	}

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

void sigintlHandler(int signal){
	printf("\n\nProgramm interupted (ctrl+c)\n");
	printf("i2c_handler stopped\n");
	printf("file closed\n");
	i2c_close(pi, i2c_handle);
	file_close(pi, file_handle);
	exit(0);
}
