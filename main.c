#include <stdio.h>
#include "floating.h"
#include <stdlib.h>


int main(int argc, char **argv){
  int i;
  union floating f;
  uint16_t half;
  char buffer[256];
  if(sizeof(float) != 4){
    printf("Error:  Can only run on a system where sizeof(float) == 32");
    return -1;
  }
  //NaN
  printf("Input: \"%s\"\n", "0x7F800001");
    f.as_int = 0x7F800001;
    printf("Info: %s\n", floating_info(f, buffer, 256));
  //INF
  printf("Input: \"%s\"\n", "0x7F800000");
    f.as_int = 0x7F800000;
    printf("Info: %s\n", floating_info(f, buffer, 256));
    //denorm
  printf("Input: \"%s\"\n", "0x00000001");
    f.as_int = 0x00000001;
    printf("Info: %s\n", floating_info(f, buffer, 256));
 //   printf("Info: %s\n", ieee_16_info(fl , buffer, 256));

  for(i = 1; i < argc; ++i){
    printf("Input: \"%s\"\n", argv[i]);
    f.as_float = strtof(argv[i], NULL);
    printf("Float: %f\n", f.as_float);
    printf("Hex: 0x%08x\n", f.as_int);
    printf("Info: %s\n", floating_info(f, buffer, 256));
   half = as_ieee_16(f);
    printf("Half Hex: 0x%04x\n", half);
    printf("Half info: %s\n", ieee_16_info(half, buffer, 256));
    printf("\n");
  }

  return 0;
}

