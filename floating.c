/*
 * Standard IO and file routines.
 */
#include <stdio.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "floating.h"

/* This function is designed to provide information about
   the IEEE floating point value passed in.  Note that this
   ONLY works on systems where sizeof(float) == 4.

   For a normal floating point number it it should have a + or -
   depending on the sign bit, then the significand in binary in the
   format of the most significant bit, a decimal point, and then the
   remaining 23 bits, a space, and then the exponent as 2^ and then a
   signed integer.  As an example, the floating point number .5 is
   represented as "+1.00000000000000000000000 2^-1" (without the
   quotation marks).

   There are a couple of special cases: 0 should be "+0" or "-0"
   depending on the sign.  An infinity should be "+INF" or "-INF", and
   a NAN should be "NaN".

   For denormalized numbers, write them with a leading 0. and then the
   bits in the denormalized value.

   It should be safe: The output should be truncated if the buffer is
   not sufficient to include all the data.
*/
char *floating_info(union floating f, char *buf, size_t buflen){
  //using float, uint interchangably
 //fprintf(stderr, "Need to implement\n");
 char *exp = malloc(sizeof(char)*4);
 char *mantissa = malloc(sizeof(char)*24);
 int i=31;
 int pow = 7;
 long int bin = 0x80000000; //comparison to extract ints, mask w/ 0's
 //shift when we need to extract more than one bit
 //two's complement, iEEE -- 1000000 is negative...
 //so if 0, it's pos
 //uint32_t is reintepretation of memory address
 if (((bin&f.as_int)>>i)==1){
  buf[0] = '-';
 } else{
  buf[0] = '+';
 }
 i=i-9; bin = bin>>9;
 fprintf(stderr, "i is: %d ", i);

 //first 10 characters of mantissa, including sign, null terminated
 //idea of this code: read bits in left to right, assigning them to the appropriate values.
int32_t exponent = (int32_t)((f.as_int&(0x7F800000))>>23);
 exponent -= 127;
 sprintf(exp,"%d",exponent);
 //printf("exponent: %d ", exponent);
  buf[1] = '1';
  int ch = 3;
while(i>=0){
  mantissa[22-i] = ((f.as_int&bin)>>i)+'0'; 
  i--;
}
mantissa[23]='\0';

i=22;
int index;
if (exponent == 128 && strcmp(mantissa,"00000000000000000000000")){
  char *form = "INF";
  for (index=1; index<buflen-2 ;index++){
      buf[index] = form[index-1];
  }
  buf[index] = '\0';
  return buf;
} else if(exponent == 128 && !strcmp(mantissa,"00000000000000000000000")){
  fprintf(stderr, "entered\n");
  char *form = "NaN";
  for (index=0; index<buflen-2 ;index++){
      buf[index] = form[index-1];
  }
  buf[index] = '\0';
  return buf;
}else if(exponent == -127 && strcmp(mantissa,"00000000000000000000000")){
  char *form = "0";
  for (index=0; index<buflen-2 ;index++){
      buf[index] = form[index-1];
  }
  buf[index] = '\0';
  return buf;
} 
if (exponent == -127){//denormalization handling here
  buf[1] = '0';
  }
  else{
  buf[1] = '1';
  } 

  buf[2]='.';
while(i>=0 && ch < buflen-2){
  buf[ch] = ((f.as_int&bin)>>i)+'0'; 
  i--; ch++;  bin=bin>>1;
}

char *format = " 2^";
i=0;
while (ch<buflen-2 && i<3){
  buf[ch] = format[i];
  i++;
  ch++;
}

for (i=0;i<(strlen(exp)+1)&&(ch)<buflen-2; i++){
  buf[ch] = exp[i];
  ch++;
}
buf[buflen-1]='\0';
return buf;
}

/* This function is designed to provide information about
   the 16b IEEE floating point value passed in with the same exact format.  */
char *ieee_16_info(uint16_t f, char *buf, size_t buflen){
  fprintf(stderr, "Need to implement\n");
  if(buflen >= 1) buf[0] = '\0';
  return buf;
}



/* This function converts an IEEE 32b floating point value into a 16b
   IEEE floating point value.  As a reminder: The sign bit is 1 bit,
   the exponent is 5 bits (with a bias of 15), and the significand is
   10 bits.

   There are several corner cases you need to make sure to consider:
   a) subnormal
   b) rounding:  We use round-to-even in case of a tie.
   c) rounding increasing the exponent on the significand.
   d) +/- 0, NaNs, +/- infinity.
 */
uint16_t as_ieee_16(union floating f){
  fprintf(stderr, "Need to implement\n");
  return 0;
}



