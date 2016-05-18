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
	
	//NEED CODE set motors to move forward until front IR sensor detects wall
	networkGate ();
}

int networkGate(){
   init(0);
   connect_to_server("130.195.6.196", 1024); //connects to server with the ip address 130.195.6.196 on port 1024
   send_to_server("Please"); //sends please to the connected server
   char message[24];
   receive_from_server(message); //receives message from the connected server
   printf("%s/n", message);
   send_to_server(message);
   printf("%s/n", "Message was sent to the server");
   
   Sleep(1,0); //waits for gate to open
   lineFollow (); //starts following line through gate
   return 0;
}

int lineFollow(){ 
	init(0);
	// This sets up the RPi hardware and ensures
	// everything is working correctly
	int sampleSize = 32;//A set variable of the array's length
	int pLine[sampleSize]; //Creates an array to store pixel values
	float errorValue, totalErrorValue = 0, prevErrorValue = 0, dErrorValue, errorDiff; //The error values for the P, I, D, D and D
	int left, right; 
	double kp = 0.2; //P value in PD controller
	double kd = 0; //D value in PD controller SET TO 0 FOR TUNING	
	double ki = 0; //I value in PD controller SET TO 0 FOR TUNING
	double timeStep = 0.2; //The time period used for calculating kd
	time_t start_t, end_t = 0; //The start and the end points for calculating the time difference
	int repetition = 10; //The amount of repeating
	bool lineFound = false; //counts bright pixels found for intersection handling

	open_screen_stream(); //Allows the camera to be displayed on the desktop

	while(true) //This creates a never ending loop
	{
		errorValue = 0;
		/** The origional line detection code
		pTot = 0;
		avg = 0;
		take_picture(); //Self explanatory
		for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel
		{
			pLine[i] = get_pixel(i*10,120,3);
			pTot += pLine[i];
		}
		avg = (float)pTot/sampleSize; //Gets average brightness of pixels
		
		for (int i = 0; i < sampleSize; i++) //If pixel is brighter than average, negative number means line is to the left, positive if line is to the right
		{
			if (pLine[i]>avg){
				errorValue += 10*(i-sampleSize/2);
			}
		}
		*/
		
		//*************************EXPERIMENTAL**************************
		for (int r = 0; r < repetition; r++)//takes 10 photos, storing the total value for each pixel
		{
			take_picture(); //Self explanatory
			for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel
			{
				pLine[i] += get_pixel(i*10,120,3);
			}
		}
		for (int i = 0; i < sampleSize; i++) //Finds the average brightness of each required pixel
		{
			pLine[i] = pLine[i]/repetition
		}
		for (int i = 0; i < sampleSize; i++) //Checks if the pixel is brighter than half brightness (making an assumption that black will be consstantly les and white consistantly more)
		{
			if (pLine[i]>127)
			{
				errorValue += 10*(i-sampleSize/2);// error is a positive value if the line is to the right, negative value if line is to the left
				lineFound = true;
			}
		}
		//*******************************END OF EXPERIMENTAL******************************
		if(!lineFound) //if robot only sees black, it calls reverse
		{
			turnAround (); //calls turnAround which rotates the robot slowly to the left until the line is found again
		}
		totalErrorValue += errorValue; //calculating the integral error value
		
		time(&start_t); //Finds the current time
		if (difftime(end_t, start_t) > timeStep) //Runs if the time period is larger than the timestep
		{
			end_t = start_t;
			//Formulas used to calculate the dErrorValue
			errorDiff = errorValue - prevErrorValue;
			dErrorValue = (float)errorDiff/timeStep;
			prevErrorValue = errorValue;
		}


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
/* commented out methods because they aren't needed, may be used in future. probably not
int reverse(void){ //sets motors to reverse for a short period of time
	setMotor(1, -50);
	setMotor(2, -50);
	Sleep(0,5000);
	turnLeft (); //calls method to turn 90 degrees left. Should then be on track and facing correct direction in any situation
	return(0);
		
}

int turnLeft(void){
	set_motor(1, -40);
	set_motor(2, 40);
	Sleep(1,0);
	lineFollow (); //calls lineFollow to check whether the line was found
	return(0);
}

int turnRight(void){ //not needed for the current line maze. could be helpful in wall maze
	set_motor(1, 40);
	set_motor(2, -40);
	Sleep(1,0);
	return(0);
}
*/
int turnAround(void)
{
	int sampleSize = 32;//A set variable of the array's length
	int pLine[sampleSize]; //Creates an array to store pixel values
	int repetition = 10;
	bool lineFound = false; //false if all pixels are black
	
	while(lineFound == false)
	{
		set_motor(1, -20);
		set_motor(2, 20)
			for (int r = 0; r < repetition; r++)//takes 10 photos, storing the total value for each pixel
			{
				take_picture(); //Self explanatory
				for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel
				{
					pLine[i] += get_pixel(i*10,120,3);
				}
			}
			for (int i = 0; i < sampleSize; i++) //Finds the average brightness of each required pixel
			{
				pLine[i] = pLine[i]/repetition
			}
			for (int i = 0; i < sampleSize; i++) //Checks if the pixel is brighter than half brightness (making an assumption that black will be consstantly les and white consistantly more)
			{
				if (pLine[i]>127)
				{
					lineFound = true;
				}
			}
		update_screen();
	}
	return(0);
}
