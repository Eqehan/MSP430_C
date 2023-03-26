
#include "io430.h"

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  P1DIR = 0x41;
  P1REN = 0x08;
  P1OUT = 0x08;
  
  while(1){
  
    if( (P1IN & 0x08) == 0)
      P1OUT = P1OUT | 0x01;
    else
      P1OUT = P1OUT & 0xFE;
  }

}
