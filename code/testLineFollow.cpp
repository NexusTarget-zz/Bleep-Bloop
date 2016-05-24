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

int main()
{
	init(0);
	// This sets up the RPi hardware and ensures
	// everything is working correctly
	int pLine[32]; //Creates an array to store pixel values
	int sampleSize = sizeof(pLine)/sizeof(int); //A set variable of the arrays length
	int pTot;
	float errorValue; //Error value that sets the distance between line and centre
	float prevErrorValue = 0; //Previous value of error used to calculate the dErrorValue
	float dErrorValue;
	float errorTot = 0;
	int left; 
	int right;
	double kp = 6; //P value in PD controller
	double kd = 0; //D value in PD controller
	double ki = 0;
	double timeStep = 0.2; //The time period used for calculating kp
	time_t start_t; //The start point for calculating a time difference
	time_t end_t = 0; //End point for calculating time difference
	bool lineFound = false;
	open_screen_stream(); //Allows the camera to be displayed on the desktop
	while(true) //This creates a never ending loop
	{
		pTot = 0;
		errorValue = 0;
		lineFound = false;
		take_picture(); //Self explanatory

		for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel	
		{
			//printf("i = %d\n", i);
			pLine[i] = get_pixel(i*10,120,3);
			pTot += pLine[i];
		}

		for (int i = 0; i < sampleSize; i++) //If pixel is brighter than average, negative number means line is to the left, positive if line is to the right
		{
			if (pLine[i]>127){
				errorValue += i-sampleSize/2;
				lineFound = true;
				printf("#");
			}
			else
			{
				printf("0");
			}
		}

		time(&start_t); //Finds the current time
		if (difftime(end_t, start_t) > timeStep)
		{
			
			end_t = start_t;
			//Formulas used to calculate the dErrorValue
			double errorDiff = errorValue - prevErrorValue;
			dErrorValue = (float)errorDiff/timeStep;
			prevErrorValue = errorValue;
		}
		
		if(!lineFound)
		{
			set_motor(1, -80);
			set_motor(2, -80);
			Sleep(0, 5000);
			set_motor(1, 40);
			set_motor(2, -40);
		}
		else
		{

			errorValue = errorValue/sampleSize; //Gets average of error
			printf("%f\n", errorValue); //%f because errorValue is a float
			// Determines the new motor speeds to alter direction
			errorTot += errorValue;
			left = 50 - (errorValue * kp) - (dErrorValue * kd) - (errorTot * ki);
			right = 50 + (errorValue * kp) + (dErrorValue * kd) + (errorTot * ki);
			// Changes the motor speeds to the predetermined values
			set_motor(1, left);
			set_motor(2, right);
			printf("left = %d, right = %d\n", left, right);
		}
		//printf("pLine = %d, %s",pLine, " ");
		//for(int i = 0; i < sampleSize; i++){
		//	printf("pLine = %d\n", pLine[i]);
		//}
		update_screen();
	}

	close_screen_stream();
	return 0;
}
