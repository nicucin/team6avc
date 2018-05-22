
#include<stdio.h>
#include<time.h>
#include"E101.h"

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
	openGate();
	return 0;
}
