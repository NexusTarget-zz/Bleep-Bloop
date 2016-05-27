#include <stdio.h>
#include <time.h>

extern "C" int init(int d_lev);
extern "C" int read_analog(int ch_adc);
extern "C" int Sleep(int sec, int usec);

int main(){
  
<<<<<<< HEAD
 init(0);
=======
  init(0);
>>>>>>> ef328dd1f8c91f4fe799d566f8afcb95a111f82a
  int adc_reading;
  while(true)
    {
      adc_reading = read_analog(0);
      printf("Front: %d", adc_reading);
      adc_reading = read_analog(1);
      printf("  Left: %d", adc_reading);
      adc_reading = read_analog(2);
      printf("  Right: %d\n", adc_reading);
      Sleep(0,999999);
    }

}
