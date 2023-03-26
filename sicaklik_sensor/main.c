
#include "io430.h"

      //Global Degiskenler
int sayilar[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
int a;
int sicaklik;

int main( void )
{
      // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  P1DIR=0x7F; 
  
  BCSCTL1 = CALBC1_1MHZ; 
  DCOCTL  = CALDCO_1MHZ;  
      //ADC ayarlari
  ADC10CTL0 = ADC10ON + ADC10IE;  
  ADC10CTL1 = INCH_10 + ADC10DIV_7;  
  ADC10AE0 = INCH_10;
  ADC10CTL0 |= ADC10SC + ENC;
  
  __enable_interrupt();
  
      //Timer ayarlari
  TA0CTL= MC_2 | ID_3 | TASSEL_2 | TACLR; 
  TACCR0 = 50000;
  TA0CCTL0 = CCIE;
  
  while(1)
  {    
      //Sonsuz Dongu
  }
      // return 0; sonsuz dongude kullanma
}

      //Timer Kesme Vektoru
#pragma vector = TIMER0_A0_VECTOR
__interrupt void timeraisr (void)
{
 sicaklik = ((ADC10MEM - 673) * 423) / 1024;
 a= sicaklik % 10;
 P1OUT=sayilar[a];
}

      //ADC Kesme Vektoru
#pragma vector = ADC10_VECTOR
__interrupt void adc10isr (void)
{
 ADC10CTL0 |= ENC + ADC10SC;
}