#include <stdio.h>
#include <stdbool.h>
#include <pigpiod_if2.h>
#include <signal.h>

#define MPU6050_ADRESS	0x68

int16_t accX, accY, accZ, gyrX, gyrY, gyrZ, tVal;
double temp = 0.0;
int pi;
int i2c_handle;

// funciton prototypes
bool init();
void readMPU6050();
void sigintlHandler(int signal);

void main(){
	signal(SIGINT, sigintlHandler); // this handler is called when ctrl+c is pressed
	
	if(init() == false){
		return -1;
	}

	wihle(true){
		readMPU6050(pi, handle);
		printf("accX: %d; accY: %d; accZ: %d; gyrX: %d; gyrY: %d; gyrZ: %d; temp: %f\n", accX, accY, accZ, gyrX, gyrY, gyrZ, temp);
		sleep(1);
	}
}


bool init(){
	// init pigpio
	pi = pigpio_start(NULL, NULL);
	if(pi < 0){
		printf("Raspberry Pi not found");
		return false;
	}
	
	// init i2c
	i2c_handle = i2c_open(pi, 1, MPU6050_ADRESS, 0);
	if(i2c_handle < 0){
		printf("i2c device not found");
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

void sigintlHandler(int signal){
	printf("signal: %d", signal);
	i2c_close(pi, i2c_handle);
	exit(0);
}