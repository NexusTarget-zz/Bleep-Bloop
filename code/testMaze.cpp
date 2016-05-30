#include <time.h>

extern "C" int init(int d_lev);
extern "C" int read_analog(int ch_adc);
extern "C" int Sleep(int sec, int usec);
extern "C" int set_motor(int motor, int speed);

int main(){
  
init(0);
  int frontIR; //current reading for each IR sensor
  int leftIR;
  int rightIR;
  int frontWall = 630; //threshold for if wall is detected in front of the robot
  int leftWall = 345; //target distance for wall to left
  int rightWall = 345; //target distance if wall to right
  double kp = 0.1;
  double errorValue = 0;
  int motorSpeed = 50;
  double left; //amount to adjust each motor speed
  double right;
  
  while(true)
    {
      frontIR = read_analog(0); //reads IR sensors and prints to the console
      printf("Front: %d", frontIR);
	  
      leftIR = read_analog(1);
      printf("Left: %d", leftIR);
	  
      rightIR = read_analog(2);
      printf("Right: %d\n", rightIR);
	  
	  if(frontIR > frontWall){ //if a wall is detected in front of the robot
	    printf("wall in front of robot detected \n");
		if(rightIR > rightWall){ //turn right if there is no wall to the right
			printf("no wall to the right of robot \n");
			set_motor(1, 0.3*motorSpeed); //tune 0.3 for arc of maze
			set_motor(2, motorSpeed);
		}if(leftIR > leftWall){ //turn left if there is no wall to the left
			printf("no wall to the left of robot \n");
			set_motor(1, motorSpeed);
			set_motor(2, 0.3*motorSpeed);
		}
	  }else{ //if there is no wall in front of the robot
		errorValue = (leftWall - leftIR)+(rightIR - rightWall); //generate negative error for too far left, positive for too far right
	  								//whether this translates to correct motor movements is yet to be seen
		right = motorSpeed - (errorValue * kp); //find motor speeds
		left = motorSpeed + (errorValue * kp);
	  
		set_motor(1, right); //set motor speeds
		set_motor(2, left);
	  }
    }

}
