#include <stdio.h>
#include <time.h>

extern "C" int read_analog(int ch_adc);
extern "C" int set_motor(int motor, int speed);
extern "C" int init(int d_lev);
extern "C" int take_picture();
extern "C" char get_pixel(int row,int col,int colour);
extern "C" int Sleep(int sec, int usec);
extern "C" int update_screen();
extern "C" int open_screen_stream();
extern "C" int close_screen_stream();
extern "C" int connect_to_server(char server_addr[15], int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);

int main ()
{
	int delay = 0;
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
	double kd = 0.0000; //D value in PD controller
	double ki = 0;
	double pixelCount;
	double timeStep = 0.1; //The time period used for calculating kp
	time_t start_t; //The start point for calculating a time difference
	time_t end_t = 0; //End point for calculating time difference
	bool lineFound;
	bool centered;
	int pixelThresh = 105;
	char message[24];
	for(int i = 0; i < 24; i++)
	{
		message[i] = 0;
	}
	
	init(0);
	connect_to_server("130.195.6.196", 1024); //connects to server with the ip address 130.195.6.196 on port 1024
	send_to_server("Please"); //sends please to the connected server
	receive_from_server(message); //receives message from the connected server
	char* response = message;
	send_to_server(response);
	while(true) //This creates a never ending loop
	{
		int frontIR; //current reading for each IR sensor
		int leftIR;
	  	int rightIR;	
  		int leftWall = 345; //target distance for wall to left
	  	int rightWall = 345; //target distance if wall to right
	  	leftIR = read_analog(1); // Reads Left IR Sensor 
	  	rightIR = read_analog(2);// Reads Right IR Sensor 
		delay++;
		if(delay>40 && (leftIR > leftWall || rightIR > rightWall)){
	
	  	int frontWall = 550; //threshold for if wall is detected in front of the robot
	  	int leftWall = 345; //target distance for wall to left
	  	int rightWall = 345; //target distance if wall to right
	  	//double kp = 0.1;
	 	//double errorValue = 0;
		//int motorSpeed = 50;
		//double left; //amount to adjust each motor speed
		//double right;
		bool intersection = false;
		bool intersection2 = false;

        while(true){
        	frontIR = read_analog(0); //Reads Front IR Sensor and prints to the console
                printf("Front: %d", frontIR);

                leftIR = read_analog(1); // Reads Left IR Sensor and prints to console
                printf(" Left: %d", leftIR);

                rightIR = read_analog(2);// Reads Right IR Sensor and prints to console
                printf(" Right: %d\n", rightIR);

                if(frontIR > frontWall){ //if a wall is detected in front of the robot
                        printf("wall in front of robot detected \n");
                        if((rightIR < rightWall) && (leftIR < leftWall)){ //turn right if there is no wall
                                printf("no wall left or right \n");
                                intersection = true;
                	        set_motor(1, 0.3 * motorSpeed); //tune 0.43 - got it from calc$
                                set_motor(2, motorSpeed);
                                
        /*      }else if((rightIR < rightWall) && (leftIR > leftWall){
                        printf("no wall to right \n");
                        set_motor(1, -1 * motorSpeed);
                        set_motor(2, motorSpeed);*/
                }

                //else if(leftIR > leftWall){ //turn left if there is no wall to the left
                 //     printf("no wall to the left of robot \n");
                 //     set_motor(1, motorSpeed);
                 //     set_motor(2, 0.30 * motorSpeed);
                //}else if(rightIR > rightWall){
                 //     printf("no wall right of robot \n");
                 //     set_motor(1, 0.30 * motorSpeed);
                 //     set_motor(2, motorSpeed);
                //}
          }else{ //if there is no wall in front of the robot
                if(intersection){
                	printf("intersection 1 started \n");
                	set_motor(1, 0.3* motorSpeed);
                        set_motor(2, motorSpeed);
                        if(rightIR > rightWall){
                        	intersection2 = true;
                        	intersection = false;
                        	printf("intersection 1 ended \n");
                        }
                }
                if(intersection2){
                	printf("intersection 2 started \n");
                	set_motor(1, 0.3* motorSpeed);
                        set_motor(2, motorSpeed);
                        if(leftIR > leftWall){
                        	intersection2 = false;
                        	printf("intersection 2 ended \n");
                        }
                }
                        
                	
                errorValue = (leftWall - leftIR)+(rightIR - rightWall); //generate negative error for to$
                                                                        //whether this translates to cor$
                right = motorSpeed - (errorValue * kp); //Calculates Right Motor Speed
                left = motorSpeed + (errorValue * kp); // Calculates Left Motor Speed
		set_motor(1, left); //Sets Left Motor Speed
                set_motor(2, right); // Sets Right Motor Speed
          }
    }
}
		pTot = 0; //Initilises variables
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
		{ //printf statements used to show a graphical representation of how the camera detects the line
			if (pLine[i]>pixelThresh)
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
				Sleep(0, 250000);
				while(!centered)
				{
					int tLine[32];
					take_picture();
					for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel	
					{
						tLine[i] = get_pixel(i*10,120,3);
					}
			
					set_motor(1, motorSpeed);
					set_motor(2, -1*motorSpeed);
					if(tLine[16]>pixelThresh)
					{
						centered = true;
					}
					
				}
			}
			else if(errorValue < 0) //if line not found or left 90deg corner/T junction detected turn left
			{
				set_motor(1, motorSpeed);
				set_motor(2, motorSpeed);
				Sleep(0, 250000);
				while(!centered)
				{
					
					int tLine[32];
					take_picture();
					for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel	
					{
						tLine[i] = get_pixel(i*10,120,3);
					}
					set_motor(1, motorSpeed);
					set_motor(2, -1*motorSpeed);
					if(tLine[16]>pixelThresh)
					{
						centered = true;
					}
					
				}
			}
			else if(errorValue > 0) 	//if right hand 90deg corner found, turn right
			{
				set_motor(1, motorSpeed);
				set_motor(2, motorSpeed);
				Sleep(0, 250000);
				while(!centered)
				{
					int tLine[32];
					take_picture();
					for (int i = 0; i < sampleSize; i++) //Finds brightness of each required pixel	
					{
						tLine[i] = get_pixel(i*10,120,3);
					}
					set_motor(1, -1*motorSpeed);
					set_motor(2, motorSpeed);
					if(tLine[16]>pixelThresh)
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
