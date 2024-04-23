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
 char mantissa[24];
 char sign;
 char integer;

 //shift when we need to extract more than one bit
 //two's complement, iEEE -- 1000000 is negative...
 //so if 0, it's pos
 //uint32_t is reintepretation of memory address
 if (buflen==0){return buf;}
 if (((0x80000000&f.as_int)>>31)==1){sign = '-';}
 else{sign = '+';}

 //first 10 characters of mantissa, including sign, null terminated
 //idea of this code: read bits in left to right, assigning them to the appropriate values.
int exponent = ((f.as_int & 0x7F800000) >> 23) - 127;
if (exponent == 128 && !(f.as_int&0x007FFFFF)){
  snprintf(buf, buflen, "%c%s", sign,"INF"); 
  return buf;
} else if(exponent == 128 && (f.as_int&0x007FFFFF)){
    snprintf(buf, buflen, "%s", "NaN");
  return buf;
}else if(exponent == -127 && !(f.as_int&0x007FFFFF)){
  snprintf(buf, buflen, "%c%s", sign,"0"); 
  return buf;
} 
if (exponent == -127){//denormalization handling here
  integer = '0';
  }
  else{
  integer = '1';
  } 
int bin = 0x400000;
int i;
for (i=0; i<23; i++){
  mantissa[i] = ((f.as_int&bin)>>(22-i))+'0'; 
  bin=bin>>1;
}
mantissa[23] = '\0';
snprintf(buf,buflen, "%c%c.%s 2^%d",sign,integer, mantissa, exponent);
return buf;
}



/* This function is designed to provide information about
   the 16b IEEE floating point value passed in with the same exact format.  */
char *ieee_16_info(uint16_t f, char *buf, size_t buflen){
  char mantissa[11];
  char sign;
  char integer;
  //problem: reading everything in as 0.

  //shift when we need to extract more than one bit
  //two's complement, iEEE -- 1000000 is negative...
  //so if 0, it's pos
  //uint32_t is reintepretation of memory address
  
  if (buflen==0){return buf;}
  fprintf(stderr, "16b input: %0x\n", (f));
  if (((0xFFFF&f) >> 14)){sign = '-';}
  else{sign = '+';}
  fprintf(stderr,"-------------------ieee_16_info call--------------\n");
  fprintf(stderr,"sign is %c\n", sign);

  //first 10 characters of mantissa, including sign, null terminated
  //idea of this code: read bits in left to right, assigning them to the appropriate values.
  int exponent = ((0x7C00 & f) >> 10) -15 ;
  fprintf(stderr, "exp: %d\n", exponent);
  int index;
  if (exponent == 16 && !(f &0x3FF)){
    snprintf(buf, buflen, "%cINF", sign); 
    return buf;
  } else if(exponent == 16 && ( f & 0x3FF)){
      snprintf(buf, buflen, "%s", "NaN");
    return buf;
  }else if(exponent == -15 && !(f & 0x3FF)){
    snprintf(buf, buflen, "%c0", sign); 
    return buf;
  } 
  if (exponent == -15){//denormalization handling here
    integer = '0';
    }
    else{
    integer = '1';
    } 
  int bin = 0x200;
  int i;
  for (i=0; i<10; i++){
    mantissa[i] = ((f & bin)>>(9-i)) +'0'; 
    bin=bin>>1;
  }
  mantissa[10] = '\0';
  snprintf(buf,buflen, "%c%c.%s 2^%d",sign,integer, mantissa, exponent);
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
  u_int16_t sign = (0x8000000 & f.as_int)>>31;
  int exponent = (0x7F80000 & f.as_int)>>23 - 127;
  u_int16_t mantissa = (f.as_int&0x007FFFFF);
  u_int8_t eleven = (f.as_int&0x1000)>>12;
  u_int8_t lsb = (f.as_int&0x2000)>>13;


  //deal with 0/denormalized/underflow numbers coming in. will be zero anyways due to reduced range. 
  //sees if exponent is all zero's. 
  if(exponent<-24){
  return sign<<15;
  }
  //deal with NaN coming in.  (all 1's in exponent, nonzero in mantissa)
  else if((exponent == 128)&&(mantissa)){
  return sign<<15 +0x7C00<<10 + mantissa>>13;
  }
  //deal with INF coming in (all 1's in exponent, all 0's in mantissa, ie 127 after debiasing) as well as overflow (anything > 15)
  else if(exponent>15){
  return sign<<15 + 0x7C00<<10;
  }
  //denormalization handling
  else if (exponent = -15){
   mantissa = mantissa >> 13; 
   if(eleven&&lsb){mantissa += 1;}
   if(mantissa & 0x400){
    exponent += 1;
    mantissa = mantissa >> 1;
   }
    return (sign<<15 + exponent<<10 +mantissa);
  }
  //now for rounding, the fun part...
  if(eleven&&lsb){
    //handle rounding up
    mantissa += 1;
    //handle overflow (return inf)
   if(mantissa & 0x400){
    exponent += 1;
    if(exponent>15){return (sign<<15 + 0x7C00<<10);} //return inf if overflow
    mantissa = mantissa >> 1;
   } 
  }
  return (sign<<15 + exponent<<10 +mantissa); 
}


