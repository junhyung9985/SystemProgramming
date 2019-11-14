#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <wiringPi.h>
char flag = 0 ;
void interrupt_handler () {
	flag++ ;
	printf("flag: %d\n", flag) ;
	if( flag %2 ==1){ 
		digitalWrite (0, HIGH) ; delay (500) ;
	}
	else{
		digitalWrite (0, LOW) ; delay (500) ;
	}
}
int main () {
	if (wiringPiSetup() < 0) exit(EXIT_FAILURE) ;
	pinMode(0,OUTPUT);
	if (wiringPiISR(22, INT_EDGE_FALLING, &interrupt_handler) < 0) {
   
		fprintf(stderr, "unsable to setup ISR\n") ;
		exit(1) ;
	 }
	while (1) ;
 }
