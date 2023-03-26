
#include "msp430.h"

void delay_ms(unsigned int ms)
{
  while(ms--)
    __delay_cycles(1000);
}

void main( void )
{
  WDTCTL = WDTPW + WDTHOLD;
  
  int a;
  
  P1DIR = 0xFF;

  while(1)
  {
   // a<<=1;
    if(a<=1){
    for(a=1;a<=64;a=a+1){
      P1OUT = a;
      delay_ms(75);
    }
    }
    if(a>=63){
    for(a=64;a>=1;a=a-1){
      P1OUT = a;
      delay_ms(75);
    }
    }
  }
}
