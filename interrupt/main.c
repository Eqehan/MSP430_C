#include "io430.h"
void bekle(unsigned int);
void hesapla(unsigned int);

unsigned int b1,b2,b3,b4,a=0,AD=1;
unsigned int sayilar[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
int main (void)
{
  WDTCTL = WDTPW + WDTHOLD;

  unsigned int i,k=500,b=0;
  
  P1DIR=0xFF;
  P2DIR=0x0F;
  
  TACCR0 = 500;           //sürenin deðiþmesi hýzlandýrýr yavaslatýr
  TACCTL0 = CCIE;
  TACTL = MC_1|ID_3 |TASSEL_2|TACLR;
  __enable_interrupt();
    hesapla (a);
  while(1)
  {
   a++;
   hesapla(a);
   bekle (1000);
  }
  return 0;
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void)
{
  if (AD == 1)
  {
    P2OUT = 0x01;
    P1OUT = sayilar[b1];
  }
  else if (AD == 2)
  { 
  P2OUT = 0x02;
  P1OUT = sayilar[b2];
  }
  else if (AD == 3)
  { 
  P2OUT = 0x04;
  P1OUT = sayilar[b3];
  }
  else if (AD == 4)
  { 
  P2OUT = 0x08;
  P1OUT = sayilar[b4];
  AD = 0;
  }
  AD++;
}

void bekle(unsigned int x)
{
  while(x--);
}

void hesapla (unsigned int sayi)
{
  b4 = sayi %10;
  b3 = (sayi % 100) / 10;
  b2 = (sayi/100)%10;
  b4 = sayi/1000;
}