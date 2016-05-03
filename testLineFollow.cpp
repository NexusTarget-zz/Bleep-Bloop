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

int main(void){
	init(0);
	// This sets up the RPi hardware and ensures
	// everything is working correctly
	int pLine[32]; //Creates an array to store pixel values
	int pTot; //Total pixel value
	float avg; //The average brightness of pixels
	float errorValue; //Error value that sets the distance between line and centre
	int left; 
	int right;
	double kp = 4.0; //P value in PD controller

	open_screen_stream(); //Allows the camera to be displayed on the desktop

	while(true){ //This creates a never ending loop
		pTot = 0;
		errorValue = 0;
		avg = 0;

		take_picture(); //Self explanatory

		for (int i = 0; i < sizeof(pLine); i++){ //Finds brightness of each required pixel
			pLine[i] = get_pixel(i*10,56,3);
			pTot += pLine[i];
		}

		avg = (float)pTot/sizeof(pLine); //Gets average of pixel

		for (int i = 0; i < sizeof(pLine); i++){ //If pixel is brighter than average, negative number means line is to the left, positive if line is to the right
			if (pLine[i]>avg){
				errorValue += 10*(i-sizeof(pLine)/2);
			}
		}

		errorValue = errorValue/sizeof(pLine); //Gets average of error
		printf("%s\n", errorValue); //printf should be %d?
		// Determines the new motor speeds to alter direction
		left = 40 + errorValue * kp;
		right = 40 - errorValue * kp;
		// Changes the motor speeds to the predetermined values
		set_motor(1, left);
		set_motor(2, right);
		printf("%s\n", left, right);

		update_screen();
	}

	close_screen_stream();
	return 0;
}

