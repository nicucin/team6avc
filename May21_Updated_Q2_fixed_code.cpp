#include<stdio.h>
#include<time.h>
#include"E101.h"

int quadrant = 1;

void checkQuadrant(int whitePix) {
	if (quadrant == 1) {
		if (whitePix>=200) {
			quadrant = 2;
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
void moveq2(int error, int whitePix){
	unsigned char v_go = 40;
	signed char dv = error * 0.004;
	int vR = v_go + dv;
	int vL = v_go - dv;
	if (whitePix <5){
		vR = vR * -1;
		vL = vL * -1;
	}
		set_motor(1,vL);
		set_motor(2,vR);
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
	//int gate = openGate();
	//if (gate == -1) {
	//	printf("Open Gate Failed: %d\n", gate)
	
	while(quadrant == 1){
		int threshold = findThreshold();
		int whi[320] = {0};
		getPicture(whi, threshold);
		int error = findError(whi);
		moveQ1(error);
		int whitePix = getWhite (whi, threshold);
		checkQuadrant(whitePix);
	}
	
	
	return 0;
}
