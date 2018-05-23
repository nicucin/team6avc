#include<stdio.h>
#include<time.h>
#include"E101.h"

void moveQ3(int error, int whitePix){
    // Same Code for Q2
	unsigned char v_go = 40;
	signed char dv = error * 0.004;
	int vR =  v_go + dv;
	int vL = v_go - dv;
	set_motor(1, vL);
	set_motor(2, vR);
    if (whitePix > 300){ //Checks to see if both left and right are white
        set_motor(1,0);
        set_motor(2,vR);
        }
    if (whitePix <5){ //No white reverse
        set_motor(1,-vL);
        set_motor(2,-vR);
        sleep1(1,0);
        }
    }

int main () {
	init();
	int quadrant = 3;
    sleep1(3,0);
	while(quadrant == 3){
		int threshold = findThreshold();
		int whi[320];
		getPicture(whi, threshold);
        int error = findError(whi);
        int numWhite = getWhite(whi, threshold);
		moveQ3(error, numWhite);
	}
	return 0;
}
