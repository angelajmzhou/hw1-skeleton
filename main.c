#include <stdio.h>
#include "floating.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
  int i;
  union floating f;
  uint16_t half;
  char *ptr;
  char buffer[256];

  if(sizeof(float) != 4){
    printf("Error:  Can only run on a system where sizeof(float) == 32");
    return -1;
  }

  for(i = 1; i < argc; ++i){
    printf("Input: \"%s\"\n", argv[i]);
    f.as_float = strtof(argv[i], NULL);
    if ((strncmp(argv[i],"0x",2) == 0) || (strncmp(argv[i],"0X",2)==0)){
      printf("input is an hex value\n");
      f.as_int = strtol(argv[1],&ptr, 0);  // assign to int part in union
    }else {
      printf ("input is a floating point\n");
      f.as_float = strtof(argv[i], NULL);   //assign to floating part in union
    }
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

