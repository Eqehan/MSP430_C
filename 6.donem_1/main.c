
#include "io430.h"

unsigned int sicaklik=0;
volatile char tick;
void sicaklik_init(void);
void sicaklik_oku(void);

void uart_putch(unsigned char temp)
{
while(!(IFG2&UCA0TXIFG));       // USCI_A0 TX Buffer hazir mi?
UCA0TXBUF=temp;
}

void serialInit(){
  
P1SEL=BIT2 + BIT1;              // p1.1=rxd p1.2=txd
P1SEL2= BIT2 + BIT1;
UCA0CTL1|=UCSSEL_2;             // SMCLK
UCA0BR0=104;                    // Baudrate=9600
UCA0BR1=0;                      // 1 MHz = 9600
UCA0MCTL=UCBRS0;                // Modulasyon UCBRSx=1
UCA0CTL1&=~UCSWRST;             // USCI durum makinesini calistirma
IE2|=UCA0RXIE;                  // VSCI_A0 RX interrupt'i baslatma
}

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  BCSCTL1=CALBC1_1MHZ;
  DCOCTL=CALDCO_1MHZ;
  __delay_cycles(100000);
  TA0CTL=TASSEL_2 + TACLR;
  TA0CCR0=50000-1;
  TA0CCTL0=CCIE;
  serialInit();
  sicaklik_init();
  TA0CTL|=MC_1;                 // Timer baslangici
  __bis_SR_register(LPM0_bits+GIE);
  
  while(1){
  }
  
}

// TimerA0 Kesme Vektoru
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TAO_ISR(void){
tick++;
  if(tick==10){
    sicaklik_oku();
    tick=0;
  }
}

// ADC10 Kesme Vektoru
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void){
  __bic_SR_register_on_exit(CPUOFF);
}

void sicaklik_init(){
ADC10CTL0&=~ENC;
ADC10CTL0=SREF_1+ADC10SHT_3+REFON+ADC10ON+MSC+ADC10IE;
ADC10CTL1=INCH_10+ADC10SSEL_0+ADC10DIV_0+CONSEQ_2;
ADC10DTC1=32;
__delay_cycles(256);
}

void sicaklik_oku(){
char i;
float yeni=0;
unsigned int sicaklik_ham,sicaklik,toplam=0;
unsigned int dma_buf[32];
ADC10SA=(unsigned int)dma_buf;
ADC10CTL0|=ENC+ADC10SC;
__bis_SR_register(CPUOFF+GIE);
for(i=0;i<32;i++)
  toplam+=dma_buf[i];
toplam>>=5;
sicaklik_ham=toplam;
yeni=(((sicaklik_ham-673)*423)/1024.0f)*100;
sicaklik=(unsigned int)yeni;
uart_putch(sicaklik/1000+48);
uart_putch(((sicaklik/100)%10)+48);
uart_putch('.');
uart_putch(((sicaklik/10)%10)+48);
uart_putch(' ');
}
