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

int findThreshold(){	
	int max = 0;
	int min = 255;
	take_picture();
	for (int i=0; i<320;i++){
		int pix = get_pixel(120,i,3);
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
int findError(){
	int threshold = findThreshold();
	int whi[320];
	take_picture();	
	
	for (int i=0; i<320; i++){				//Coverts vision into array of black or white (1,0)
		unsigned char pix = get_pixel(120, i, 3);
		if (pix>=threshold) {
			whi[i] = 1;			
		}
		else {
			whi[i] = 0;
		}
	}
	
	int err = 0;
	int np = 0;  						//Normalize pixels
	for (int i=0; i<320; i++){
		err = err + (i-160)*whi[i];		//Error Signal (greater than 160 positive - we are left of track)
		if (whi[i] == 1){
			np++;
		}
	}
	if (np == 0) {
		checkQuadrant(np);
		return err;
	}
	checkQuadrant(np);
	err = err/np;
	return err;
}


void move(int error){
	unsigned char v_go = 40;
	signed char dv = error * 0.004;
	int vR =  v_go + dv;
	int vL = v_go - dv;
	set_motor(1, vL);
	set_motor(2, vR);
}
int openGate(){										//Need fixing
	char serverAddr[15] = ('1','3','0','.','1','9','5','.','6','.','1','9','6', '\0');
	char message[24] = ('P','l','e','a','s','e', '\0');
	
	int connect = connect_to_server(serverAddr, 1024);
	if (connect != 0) {
		printf("Connection not established: %d", connect);
		return -1;
	}
	
	int sendMsg = send_to_server(message);
	if (sendMsg != 0) {
		printf("Failure sending request: %d", connect);
		return -1;
	}
	
	int received[] = receive_from_server(message); //Check the return type (if array)
	return 0; 										//*Temporary* so it compiles
}
	
int main () {
	init();
	int gate = openGate();
	if (gate == -1) {
		printf("Open Gate Failed: %d\n", gate);
	}
	
	while(1){
		if (quadrant == 1){
			move(findError());
		}
		if (quadrant == 2){
			
		}

	}
	
	
	return 0;
}
