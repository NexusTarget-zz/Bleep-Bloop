#include <stdio.h>
#include <time.h>

extern "C" int set_motor(int motor, int speed);
extern "C" int init(int d_lev);
extern "C" int take_picture();
extern "C" char get_pixel(int row,int col,int colour);
extern "C" int Sleep(int sec, int usec);
extern "C" int update_screen();
extern "C" int open_screen_stream();
extern "C" int close_screen_stream();

int main(){
	init(0);
	// This sets up the RPi hardware and ensures
	// everything is working correctly
	int pLine[32]; //Creates an array to store pixel values
	int pTot; //Totel pixel value
	float max; //The value a pixel must be greater
	float errorValue; //Error value that sets the distance between line and centre
	int left; 
	int right;

	open_screen_stream(); //Allows the camera to be displayed on the desktop

	while(true){ //This creates a never ending loop
		pTot = 0;
		max = 0;
		errorValue = 0;

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
		printf("%s\n", errorValue);

		left = 40 + errorValue * 4.0;
		right = 40 - errorValue * 4.0;

		set_motor(1, left);
		set_motor(2, right);
		printf("%s\n", left, right);

		update_screen();
	}

	close_screen_stream();
}

