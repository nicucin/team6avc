#include<stdio.h>
#include<time.h>
#include"E101.h"
// Starting quadrant = 1
int quadrant = 1;

int openGate(){
	char serverAddr[15] = {'1','3','0','.','1','9','5','.','6','.','1','9','6'};
	char message[24] = {'P','l','e','a','s','e'};	
	connect_to_server(serverAddr, 1024);
	send_to_server(message);
	receive_from_server(message);
	send_to_server(message);	
	return 0;
}
	
void checkQuadrant(int whitePix) {
	if (quadrant == 2) {
		if (whitePix>=200) {
			quadrant = 3;
		}
	}
}

//Reads image and returns number of white pixels
int getWhite(int whi[],int threshold){
	int whitePix = 0;
	for (int i=0;i<320;i++){
		unsigned char pix = get_pixel(120,i,3);
		if (pix>=threshold){
			whitePix++;
		}
	}
	return whitePix;
}

//Finds middle threshold unless threshold is less than 100, returns 100
int findThreshold(){	
	int max = 0;
	int min = 255;
	take_picture();
	for (int i=0; i<320;i++){
		unsigned char pix = get_pixel(120,i,3);
        if ( pix > max) 
        {
			max = pix;
		}
		if (pix < min)
		{
			min = pix;
		}
	}
	int threshold = ((max+min)/2) + 10;
	if (threshold < 100){
		threshold = 100;
		}
	return threshold;
	
}

//Returns black and white array from image
void getPicture(int *whi, int threshold){
	take_picture();
	for (int i=0; i<320; i++){
		unsigned char pix = get_pixel(120, i, 3);
		if (pix>=threshold){
			whi[i] = 1;
		}
		else {
			whi[i]=0;
		}	
	}
}

//Error Signal (greater than 160 positive - we are left of track)
int findError(int whi[]){	
	int err = 0;
	for (int i=0; i<320; i++){
		err = err + (i-160)*whi[i];		
		}
	return err;
}

//Follow line
void moveQ2(int error){
	unsigned char v_go = 40;
	signed char dv = error * 0.004;
	int vR =  v_go + dv;
	int vL = v_go - dv;
	set_motor(1, vL);
	set_motor(2, vR);
}

//Follow white line in q3
void moveQ3(int whi[], int whitePix){
	int leftWhi = 0; 
	int rightWhi = 0;
	if (whitePix >20){	//If number white pixels is greater than 5, drive forward
		printf("See white");
		set_motor(1,50);
		set_motor(2,50);
		sleep1(0,500000);
	}
	else{ //If cant see white
		while(whitePix < 10){ //Loop while cant see white
			printf("cant see white");
			int threshold = findThreshold();
			whi[320] = {0};
			getPicture(whi, threshold);
			whitePix = getWhite (whi, threshold);
			set_motor(1,-30); //Reverse
			set_motor(2,-30);
			sleep1(0,500000);
			}	
		for(int i = 0; i<159; i++){ //Count white pixels to left
			printf("counting pixels");
			if (whi[i] = 1)
				leftWhi++;	
		}
		for(int i = 160; i<320; i++){ //Count white pixels to right
			if (whi[i] = 1)
				rightWhi++; 
		}
	
		if (leftWhi > rightWhi ||(leftWhi > 140 && rightWhi > 140)){ //If left is greater or they are equal turn left
			set_motor(1,0);
			set_motor(2,50);
			sleep1(0,500000);
		}
		else if (rightWhi > leftWhi){ //If right is greater turn right
			set_motor(1,50);
			set_motor(2,0);
			sleep1(0,500000);
		}
	}
}

int main () {
	init();
	int gate = openGate(); //Call open gate
	sleep1(0,50000); // Wait for gate
	set_motor(1,60);
	set_motor(2,60);
	sleep1(1,0);
	quadrant = 2;
	//Follow line based on error
	while(quadrant == 2){
		int threshold = findThreshold();
		int whi[320];
		getPicture(whi, threshold);
		int error = findError(whi);
		moveQ2(error);
		int whitePix = getWhite (whi, threshold);
		checkQuadrant(whitePix);
	}
	while(quadrant == 3){
		printf("Start q3");
		//set_motor(1,40); ??
		//set_motor(2,40);  ??
		int threshold = findThreshold();
		int whi[320];
		getPicture(whi, threshold);
		int whitePix = getWhite (whi, threshold);
		moveQ3(whi,whitePix);
	}
	return 0;
}
