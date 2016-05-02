#include <stdio.h>
#include <time.h>


extern "C" int init(int d_lev);
extern "C" int connect_to_server(char server_addr[15], int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);
extern "C" int take_picture();
extern "C" char get_pixel(int row,int col,int colour);
extern "C" int Sleep(int sec, int usec);
extern "C" int open_screen_stream();
extern "C" int close_screen_stream();
extern "C" int update_screen();

 
int networkGate(){
	// This sets up the RPi hardware and ensures
	// everything is working correctly
	init(0);
    connect_to_server("130.195.6.196", 1024); //connects to server with the ip address 130.195.6.196 on port 1024
    send_to_server("Please"); //sends please to the connected server
    char message[24];
    receive_from_server(message); //receives message from the connected server
    printf("%s/n", message);
    send_to_server(message);
    printf("%s/n", "Message was sent to the server");
	return 0;
}

int lineFollow(){
	init(0);
	// This sets up the RPi hardware and ensures
	// everything is working correctly
	int pLine[40] //Creates an array to store pixel values
	int pTot; //Totel pixel value
	float max; //The value a pixel must be greater
	float errorValue; //Error value that sets the distance between line and centre
	int left; 
	int right;

	open_screen_stream(); //Allows the camera to be displayed on the desktop

	while(true){ //This creates a never ending loop
		int pTot = 0;
		float max = 0;
		float errorValue = 0;

		take_picture(); //Self explainitory

		for (int i = 0; i < 32; i++){ //Loads total brightness
			pLine[i] = get_pixel(i*10,56,3);
			pTot += pLine[i];
		}

		max = (float)pTot/32.0; //Gets average of pixel

		for (int i = 0; i < 32; i++){ //If pixel is brighter then average
			if (pLine[i]>max){
				errorValue += 10(i-16);
			}
		}

		errorValue = errorValue/32; //Gets average of error

		left = 40 + errorValue * 4.0;
		right = 40 - errorValue * 4.0;

		set_motor(1, left);
		set_motor(2, right);

		update_screen();
	}

	close_screen_stream();
}
