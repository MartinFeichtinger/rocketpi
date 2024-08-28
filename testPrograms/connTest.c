#include <stdio.h>
#include <stdbool.h>
#include <pigpiod_if2.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NET_LED	27

bool checkConn();

int main(){
	// init gpio client
	int pi = pigpio_start(NULL, NULL);
	if(pi < 0){
		printf("Raspberry Pi not found");
		return -1;
	}

	// init gpios
	set_mode(pi, NET_LED, PI_OUTPUT);
	gpio_write(pi, NET_LED, 0);

	while(true){
		if(checkConn() == true){
			gpio_write(pi, NET_LED, 1);
		}
		else{
			gpio_write(pi, NET_LED, 0);
		}

		time_sleep(1);
	}

	return 0;
}

bool checkConn(){
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr = { AF_INET, htons(80), inet_addr("216.58.218.238") };
	if(connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) != 0){
		close(sockfd);
		return false;
	}
	else{
		close(sockfd);
		return true;
	}
}
