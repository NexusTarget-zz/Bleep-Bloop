# include <stdio.h>
# include <time.h>
//these load specific methods from the ENGR101 library
extern "C" int init(int d_lev);
extern "C" int Sleep( int sec , int usec );
extern "C" int set_motor( int motor , int speed);

int main (){
   // This sets up the RPi hardware and ensures everything is working correctly.
   init(0);
   // Sets the motor connected to pin 1 and pin 2 to rotate in the forward direction at half speed for 3 seconds.
   set_motor(1, 127);
   set_motor(2, 127);
   Sleep (3 ,000000);
   // Sets the motor connected to pin 1 and pin 2 to rotate in the reverse direction at half speed for 3 seconds.
   set_motor(1, -127);
   set_motor(2, -127);
   Sleep(3 ,000000);
   // Stops both motors.
   set_motor(1, 0);
   set_motor(2, 0);
   return 0;
}
