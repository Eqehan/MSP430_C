

#include "msp430.h"

void delay_ms(unsigned int ms)
{
  while(ms--)
    __delay_cycles(1000);
}

void main( void )
{
  int a=1;
  
  WDTCTL = WDTPW + WDTHOLD;
  
  P1DIR = 0xFF;

  while(1)
  {
    P1OUT = a;
    a<<=1;
   
    delay_ms(75);
    
    if(a > 64)
      a=1;
  }
}
