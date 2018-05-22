#include<stdio.h>
#include<time.h>
#include"E101.h"

int quadrant = 1;

void checkQuadrant(int whitePix) {
	if (quadrant == 1) {
		if (whitePix>=200) {
			quadrant = 3;
		}
	}
}
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
	return threshold;
	
}
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
int findError(int whi[]){	
	int err = 0;
	for (int i=0; i<320; i++){
		err = err + (i-160)*whi[i];		//Error Signal (greater than 160 positive - we are left of track)
		}
	return err;
}


void moveQ1(int error){
	unsigned char v_go = 40;
	signed char dv = error * 0.004;
	int vR =  v_go + dv;
	int vL = v_go - dv;
	set_motor(1, vL);
	set_motor(2, vR);
}
void moveq3(int,whi[], int whitePix){
	int leftWhi = 0;
	int rightWhi = 0;
	if (whitePix <5){
		set_motor(1,50);
		set_motor(2,-50);
		sleep1(1,0);
	}
	else{
		for(int i = 0; i<159; i++){
			if (whi[i] = 1)
				leftWhi++;
		}
		for(int i = 160; i<320; i++){
			if (whi[i] = 1)
				rightWhi++;
		}
	}
	if (leftWhi > rightWhi){
		set_motor(1,0);
		set_motor(2,50);
		sleep1(0,500000);
	}
	else if (rightWhi > leftWhi){
		set_motor(1,50);
		set_motor(2,0);
		sleep1(0,500000);
	}
	
}

int openGate(){
	char serverAddr[15] = {'1','3','0','.','1','9','5','.','6','.','1','9','6'};
	char message[24] = {'P','l','e','a','s','e'};	
	connect_to_server(serverAddr, 1024);
	send_to_server(message);
	receive_from_server(message);
	send_to_server(message);	
	return 0;
}
	
int main () {
	init();
	int gate = openGate();
	while(quadrant == 1){
		int threshold = findThreshold();
		int whi[320] = {0};
		getPicture(whi, threshold);
		int error = findError(whi);
		moveQ1(error);
		int whitePix = getWhite (whi, threshold);
		checkQuadrant(whitePix);
	}
	while(quadrant == 3){
		set_motor(1,40);
		set_motor(2,40);
		int threshold = findThreshold();
		int whi[320] = {0};
		getPicture(whi, threshold);
		int whitePix = getWhite (whi, threshold);
		moveQ2(whi,whitePix);
	}
	return 0;
}
