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
	int motorSpeed = 50;
	double kp = 3; //P value in PD controller
	double kd = 0.0005; //D value in PD controller
	double ki = 0;
	double pixelCount;
	double timeStep = 0.1; //The time period used for calculating kp
	time_t start_t; //The start point for calculating a time difference
	time_t end_t = 0; //End point for calculating time difference
	bool lineFound;
	bool centered;
	while(true) //This creates a never ending loop
	{
		pTot = 0;
		pixelCount = 0;
		errorValue = 0;
		lineFound = false;
		centered = false;
		take_picture(); //Self explanatory

		for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel	
		{
			pLine[i] = get_pixel(i*10,120,3);
			pTot += pLine[i];
		}

		for (int i = 0; i < sampleSize; i++) //If pixel is brighter than average, negative number means line is to the left, positive if line is to the right
		{
			if (pLine[i]>95)
			{
				pixelCount++;
				errorValue += i-sampleSize/2;
				lineFound = true;
				printf("#");
			}
			else
			{
				printf("0");
			}
		}
		
		if(pixelCount == 0)
		{
			pixelCount = 1;
		}
		errorValue = errorValue/pixelCount;
		time(&start_t); //Finds the current time
		if (difftime(start_t, end_t) > timeStep)
		{
			end_t = start_t;
			//Formulas used to calculate the dErrorValue
			double errorDiff = errorValue - prevErrorValue;
			dErrorValue = (float)errorDiff/timeStep;
			prevErrorValue = errorValue;
		}
		// Determines the new motor speeds to alter direction
		errorTot += errorValue;
		right = motorSpeed - (errorValue * kp) - (dErrorValue * kd) - (errorTot * ki);
		left = motorSpeed + (errorValue * kp) + (dErrorValue * kd) + (errorTot * ki);
		if(errorValue > 0)
		{
			printf(" --- Right\n");
		}
		else if(errorValue < 0)
		{
			printf(" --- Left\n");
		}
		else
		{
			printf(" --- Centre\n");
		}
		
		if(pixelCount >= 18)
		{
			if(pixelCount >= 30)
			{
				set_motor(1, motorSpeed);
				set_motor(2, motorSpeed);
				Sleep(0, 500000);
				while(!centered)
				{
					int tLine[32];
					take_picture();
					float tErrorValue = 0;
					int tPixelCount = 0;
					for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel	
					{
						tLine[i] = get_pixel(i*10,120,3);
					}
			
					for (int i = 0; i < sampleSize; i++) //If pixel is brighter than average, negative number means line is to the left, positive if line is to the right
					{
						if (tLine[i]>95)
						{
							tPixelCount++;
							tErrorValue = i-sampleSize/2;
						}
					}
					set_motor(1, motorSpeed);
					set_motor(2, -1*motorSpeed);
					if(tErrorValue == 0 && tPixelCount >=4)
					{
						centered = true;
					}
					
				}
			}
			else if(errorValue < 0) //if line not found or left 90deg corner/T junction detected turn left
			{
				set_motor(1, motorSpeed);
				set_motor(2, motorSpeed);
				Sleep(0, 500000);
				while(!centered)
				{
					
					int tLine[32];
					take_picture();
					float tErrorValue = 0;
					int tPixelCount = 0;
					for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel	
					{
						tLine[i] = get_pixel(i*10,120,3);
					}
			
					for (int i = 0; i < sampleSize; i++) //If pixel is brighter than average, negative number means line is to the left, positive if line is to the right
					{
						if (tLine[i]>95)
						{
							tPixelCount++;
							tErrorValue = i-sampleSize/2;
						}
					}
					set_motor(1, motorSpeed);
					set_motor(2, -1*motorSpeed);
					if(tErrorValue == 0 && tPixelCount >=4)
					{
						centered = true;
					}
					
				}
			}
			else if(errorValue > 0) 	//if right hand 90deg corner found, turn right
			{
				set_motor(1, motorSpeed);
				set_motor(2, motorSpeed);
				Sleep(0, 500000);
				while(!centered)
				{
					int tLine[32];
					take_picture();
					float tErrorValue = 0;
					int tPixelCount = 0;
					for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel	
					{
						tLine[i] = get_pixel(i*10,120,3);
					}
			
					for (int i = 0; i < sampleSize; i++) //If pixel is brighter than average, negative number means line is to the left, positive if line is to the right
					{
						if (tLine[i]>95)
						{
							tPixelCount++;
							tErrorValue = i-sampleSize/2;
						}
					}
					set_motor(1, -1*motorSpeed);
					set_motor(2, motorSpeed);
					if(tErrorValue == 0 && tPixelCount >=4)
					{
						centered = true;
					}
					
				}
			}
		}
		else if(!lineFound)
		{
			if(prevErrorValue >= 0)
			{
				set_motor(1, -1*motorSpeed);
				set_motor(2, motorSpeed);
			}
			else if(prevErrorValue < 0)
			{
				set_motor(1, motorSpeed);
				set_motor(2, -1*motorSpeed);
			}
		}
		else
		{
			
			// Changes the motor speeds to the predetermined values
			set_motor(1, right);
			set_motor(2, left);
		}
	}
	
	return 0;
}
