#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pigpiod_if2.h>
#include <signal.h>

#define MPU6050_ADRESS	0x68
#define OUTPUT_FOLDER	"/home/pi/rocketpi/testPrograms/testFiles/"

uint32_t timestamp;
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
		char str[300];
		readMPU6050(pi, i2c_handle);
		sprintf(str, "%d	%d	%d	%d	%d	%d	%d	%.2f\n", timestamp, accX, accY, accZ, gyrX, gyrY, gyrZ, temp);
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
                        return -1;
                }

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
		char init_headline[] = {"time	accX	accY	accZ	gyrX	gyrY	gyrZ	temp\n"};

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

	timestamp = get_current_tick(pi)/1000;
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
