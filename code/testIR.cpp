#include <stdio.h>
#include <time.h>

//extern "C" int InitHardware(); //used this on assignment 5??
extern "C" int init(int d_lev);
extern "C" int read_analog(int ch_adc);
extern "C" int Sleep(int sec, int usec);

int main(){
  
 init(0);
  //InitHardware();
  int adc_reading;
  while(true)
    {
      adc_reading = read_analog(2);
      printf("%d\n", adc_reading);
      Sleep(0,500);
    }

}
