
#include "io430.h"
#include "intrinsics.h"

//Global degiskenler
int a;
int sayilar[6]={0x3F, 0x06, 0x5B, 0x4F,0x66,0x6D};

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  P1DIR = 0x7F;
  ADC10CTL0  = ADC10ON + ADC10IE;
  ADC10CTL1 = ADC10DIV_7 + INCH_7;
  ADC10AE0 = INCH_7; 
  ADC10CTL0 |= ENC + ADC10SC;
  
  __enable_interrupt();
  
  while(1)
  {
    P1OUT=sayilar[a]; 
  }
  
}

//ADC kesme vektoru
#pragma vector = ADC10_VECTOR
__interrupt void adasdas(void)
{
  ADC10CTL0 |= ENC + ADC10SC; 
  a = ((ADC10MEM * 5)/1024);
}