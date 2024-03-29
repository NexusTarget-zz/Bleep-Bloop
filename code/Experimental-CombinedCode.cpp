#include <stdio.h>
#include <time.h>

extern "C" int init(int d_lev);
extern "C" int connect_to_server(char server_addr[15], int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);

extern "C" int set_motor(int motor, int speed);
extern "C" int init(int d_lev);
extern "C" int take_picture();
extern "C" char get_pixel(int row,int col,int colour);
extern "C" int Sleep(int sec, int usec);
extern "C" int update_screen();
extern "C" int open_screen_stream();
extern "C" int close_screen_stream();



int networkGate(){
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
	int pLine[32]; //Creates an array to store pixel values
	int sampleSize = sizeof(pLine)/sizeof(int); //A set variable of the arrays length
	int pTot; //Total pixel value
	float avg; //The average brightness of pixels
	float errorValue; //Error value that sets the distance between line and centre
	float totalErrorValue = 0;
	float prevErrorValue = 0; //Previous value of error used to calculate the dErrorValue
	float dErrorValue; //Error value used by the derivative value kd
	int left; 
	int right;
	double kp = 0.2; //P value in PD controller
	double kd = 4.0; //D value in PD controller
	double ki = 4.0; //I value in PD controller
	double timeStep = 0.2; //The time period used for calculating kp
	time_t start_t; //The start point for calculating a time difference
	time_t end_t = 0; //End point for calculating time difference
	int repetition = 10;

	open_screen_stream(); //Allows the camera to be displayed on the desktop

	while(true){ //This creates a never ending loop
		errorValue = 0;
		/**
		pTot = 0;
		avg = 0;
		take_picture(); //Self explanatory
		for (int i = 0; i < sampleSize; i++){ //Finds brightness of each required pixel
			pLine[i] = get_pixel(i*10,120,3);
			pTot += pLine[i];
		}
		avg = (float)pTot/sampleSize; //Gets average brightness of pixels
		
		for (int i = 0; i < sampleSize; i++){ //If pixel is brighter than average, negative number means line is to the left, positive if line is to the right
			if (pLine[i]>avg){
				errorValue += 10*(i-sampleSize/2);
			}
		}
*/

//*************************EXPERIMENTAL**************************
		for (int r = 0; r < repetition; r++)//takes 10 photos, storing the total value for each pixel
		{
			take_picture(); //Self explanatory
			for (int i = 0; i < sampleSize; i++){ //Finds brightness of each required pixel
				pLine[i] += get_pixel(i*10,120,3);
			}
		}
		for (int i = 0; i < sampleSize; i++){ //Finds the average brightness of each required pixel
				pLine[i] = pLine[i]/repetition
			}
		for (int i = 0; i < sampleSize; i++){ //Checks if the pixel is brighter than half brigtness (making an assumption that black will be consstantly les and white consistantly more)
			if (pLine[i]>127){
				errorValue += 10*(i-sampleSize/2);//adds a positive value if the line is to the right, negative value if line is to the left
			}
		}
//*******************************END OF EXPERIMENTAL******************************
		
		totalErrorValue += errorValue; //calculating the intergral error value
		
		time(&start_t); //Finds the current time
		if (difftime(end_t, start_t) > timeStep) //Runs if the time period is larger than the timestep
		{
			
			end_t = start_t;
			//Formulas used to calculate the dErrorValue
			double errorDiff = errorValue - prevErrorValue;
			dErrorValue = (float)errorDiff/timeStep;
			prevErrorValue = errorValue;
		}
		
		/* Reverse
		if(errorValue == 0){
			setMotor(1, -50);
			setMotor(2, -50);
			Sleep(0, 5000);
		}
		*/
		
		printf("%f\n", errorValue); //%f because errorValue is a float
		// Determines the new motor speeds to alter direction
		left = 40 + (errorValue * kp) + (dErrorValue *kd) + (totalErrorValue * ki);
		right = 40 - (errorValue * kp) - (dErrorValue *kd) - (totalErrorValue * ki);
		// Changes the motor speeds to the predetermined values
		set_motor(1, left);
		set_motor(2, right);
		printf("%d, %d\n", left, right);

		update_screen();
	}

	close_screen_stream();
	return 0;
}

