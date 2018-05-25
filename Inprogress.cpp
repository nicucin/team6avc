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


//Reads image and returns number of white pixels
int getWhite(int threshold){
	int whitePix = 0;
	take_picture();
	for (int i=0;i<320;i++){
		unsigned char pix = get_pixel(120,i,3);
		if (pix>=threshold){
			whitePix++;
		}
	}
	return whitePix;
}
//Reads image and returns number of red pixels
int getRed(int threshold){
	take_picture();
	int redPix = 0;
	for (int i=0;i<320;i++){
		unsigned char Rpix = get_pixel(120,i,0);		
		unsigned char Bpix = get_pixel(120,i,2);
		if ((Rpix > 200) && (Bpix < 100)){
			redPix++;
		}
	}
	return redPix;
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
	int threshold = findThreshold();
	int whitePix = getWhite(threshold);
	if (whitePix < 5){
		set_motor(1, -40);
		set_motor(2, -40);
		sleep1(1,0);
	}
	unsigned char v_go = 38;
	signed char dv = error * 0.0038;
	int vR =  v_go + dv;
	int vL = v_go - dv;
	printf("vL = %d\n", vL);
	printf("vR = %d\n", vR);
	set_motor(1, vL);
	set_motor(2, vR);
}

//Follow white line in q3
void moveQ3(int error, int whitePix){
	printf("B\n");	
	if ((whitePix < 100) && (whitePix > 5)){ // normal line
		printf("%d\n", whitePix);
		unsigned char v_go = 40;
		signed char dv = error * 0.004;
		int vR =  v_go + dv;
		int vL = v_go - dv;
		set_motor(1, vL);
		set_motor(2, vR);
	}
	else if (whitePix > 100){ // dealing with intersections
		set_motor(1, 40);
		set_motor(2, 40);
		while (whitePix > 100){
			sleep1(0, 50000);
			int threshold = findThreshold();
			whitePix = getWhite(threshold);
		}
	}
	else{ // no white line
		set_motor(1,0);
		set_motor(2,0);
		sleep1(0, 500000);
		while(whitePix < 5){
			set_motor(1, -40);
			set_motor(2, -40);
			sleep1(0, 5000);
			int threshold = findThreshold();
			whitePix = getWhite(threshold);			
			}
		set_motor(1, 0);
		set_motor(2, 0);
		sleep1(0, 5000);		
		int threshold = findThreshold();		
		whitePix = getWhite(threshold);
		int whi[320];
		getPicture(whi, threshold);
		int err = findError(whi);
		printf("err: %d\n", err);
		if (err < 400){
			set_motor(2, 0);
			set_motor(1, 40);
			sleep1(0, 300000);
		}
		else{
			set_motor(2, 40);
			set_motor(1, 0);
			sleep1(0,300000);	
		}		
	}
}	

void checkQuadrant(int Pix) {
	if (quadrant == 2) {
		if (Pix>=250) { // dealing with thick line
			quadrant = 3;
			set_motor(1,0);
			set_motor(2,0);
			sleep1(1, 0);
			set_motor(1, 50);
			set_motor(2, 50);
			sleep1(0, 250000);
			printf("Quadrant: 3\n");
		}
	else if (quadrant == 3){
		if (Pix>=50){
			quadrant = 4;
			}
		}
	}
}

int main () {
	init();
	printf("Quadrant: 1\n");
	while (quadrant == 1){
		openGate(); //Call open gate
		sleep1(0,50000); // Wait for gate
		set_motor(1,60);
		set_motor(2,60);
		printf("GAS IT");
		sleep1(1,0);
		set_motor(1,0);
		set_motor(2,0);
		quadrant = 2;
	}
	
	printf("Quadrant: 2\n");
	//Follow line based on error
	while(quadrant == 2){
		int threshold = findThreshold();
		int whi[320];
		getPicture(whi, threshold);
		int error = findError(whi);
		moveQ2(error);
		int whitePix = getWhite(threshold);
		checkQuadrant(whitePix);
	}
	while(quadrant == 3){
		sleep1(1,0);
		int threshold = findThreshold();
		int whi[320];
		getPicture(whi, threshold);
		int error = findError(whi);
		int whitePix = getWhite(threshold);
		moveQ3(error, whitePix);
		int redPix = getRed(threshold);
		checkQuadrant(redPix);
	}
	return 0;
}
