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
  if (buflen==0){return buf;}
  //fprintf(stderr, "16b input: %0x\n", (f));
  if ((f >> 15)&1){sign = '-';}
  else{sign = '+';}
  printf("-------------------ieee_16_info call--------------\n");
  printf("sign is %c\n", sign);

  //first 10 characters of mantissa, including sign, null terminated
  //idea of this code: read bits in left to right, assigning them to the appropriate values.
  int exponent = ((0x7C00 & f) >> 10) -15 ;
  //fprintf(stderr, "exp: %d\n", exponent);
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
    uint8_t sign = (f.as_int >> 31) & 1;
    int exponent = ((f.as_int >> 23) & 0xFF) - 127;
    uint32_t mantissa = f.as_int & 0x007FFFFF;

    fprintf(stderr, "16b ieee exponent value: %d\n", exponent);    
   //deal with 0/denormalized/underflow numbers coming in. will be zero anyways due to reduced range. 
    if(exponent < -24){
        return sign << 15; // only need the sign bit, the rest are zeros
    }
  //deal with NaN coming in.  (all 1's in exponent, nonzero in mantissa)
    else if((exponent == 128) && (mantissa)){
      fprintf(stderr, "NaN\n");
        return (sign << 15) + 0x7FFF ; //signal NaN
    }
  //deal with INF coming in (all 1's in exponent, all 0's in mantissa, ie 127 after debiasing) as well as overflow (anything > 15)
    else if(exponent > 15){
        return (sign << 15) + 0x7C00; // set exponent bits to all ones, mantissa to zero (infinity)
    }
  //denormalization handling
    else {
        if (exponent >= -14) {
            //biases exponent 
            exponent += 15;
        } else {
          //denormalized!!! watch out, this is still unbiased. i.e. -15 to 16 range. 
          //need to add in implied 1...
            int shift = -(exponent+14); 
            //find difference btwn 14 and the the current exponent to determine bitshift. 
              mantissa = mantissa | 0x800000; //puts implied leading 1 in
               mantissa = mantissa >>(shift); // shift mantissa to account for the exponent being STRICTLY -14. 
              exponent = 0;//exponent is 0 for denorm numbers. 
              fprintf(stderr, "new mantissa %x\n", mantissa);

        }
        u_int8_t round_bit = (mantissa>>12)&1;
        fprintf(stderr,"round bit is: %d \n", round_bit);

        // rouna
       if(round_bit && (((mantissa>>13) & 1) || (f.as_int & 0xFFF)>0)){//check if mantissa's last value is 1 and if the rounding bit is one. 
       //also checks if the bits after the round bit if the round bit is on 
        mantissa >>= 13; // bitshift mantissa to 10b form. 
        fprintf(stderr, "%0x rounded up\n", mantissa);
            mantissa += 1; //if so, round
          fprintf(stderr, "%0x after round\n", mantissa);
            if (mantissa & 0x400) { // overflow check
                fprintf(stderr, "%d is overflow\n", mantissa);
                mantissa = 0; //overflow means the number was gonna be all 0 anyways (like 111+1) so just reset to fit it. 
                exponent += 1; // increment exponent
                if (exponent > 30) { //is exponent out of bounds?
                    return (sign << 15) | 0x7C00; // return infinity
                }
            }
        }

    // Combine sign, exponent, and mantissa into the final 16-bit half-precision float
    return (sign << 15) | (exponent << 10) | (mantissa & 0x3FF);
  }

}