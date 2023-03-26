#include "io430.h"

//------------------------------------------------------------------------------
// Hardware-related definitions
//------------------------------------------------------------------------------

#ifndef UART_TXD
#define UART_TXD   0x02 		// TXD on P1.1 (Timer0_A.OUT0)
#endif
 
#ifndef UART_RXD
#define UART_RXD   0x04                 // RXD on P1.2 (Timer0_A.CCI1A)
#endif

//------------------------------------------------------------------------------
// Conditions for 9600 Baud SW UART, SMCLK = 1MHz
//------------------------------------------------------------------------------

#define CLK 1000000 
#ifndef BAUND
#define BAUND 9600
#endif
 
#define UART_TBIT_DIV_2     (CLK / (BAUND * 2))
#define UART_TBIT           (CLK / BAUND) 
 
//------------------------------------------------------------------------------
// Global variables used for full-duplex UART communication (Global Deðiþkenler)
//------------------------------------------------------------------------------ 
 
unsigned int txData;                        // UART internal variable for TX
unsigned char rxBuffer;                     // Received UART character 
 
//------------------------------------------------------------------------------
// Function prototypes (Fonksiyonlar)
//------------------------------------------------------------------------------

void UARTInit(void);
void SendByte(unsigned char byte);
void SendStr(char *string);
unsigned char RecvByte(void);
void RecvStr(char *msg, unsigned char n);
void RecvStrTEMP(void);

void uart(void)
{

 
    UARTInit();                     // Start Timer_A UART  (Timer_A UART Baþladý)
 
    SendStr("Merhaba\r\n"); 		// Karþýlama Mesajlarý Gönderiliyor...
    SendStr("UART Kullanima Hazir.\r\n");
    SendStr("GPS ile Konum Belirleme Projesi...\r\n");
    SendStr("ORHAN YILMAZ\r\n\r\n\r\n");
    
}

//-----------------------------------------------------------------------------------
// Function configures Timer_A for full-duplex UART operation (UART Ayarlarý)
//-----------------------------------------------------------------------------------

void UARTInit(void)
{
    TACCTL0 = OUT;                          // Set TXD Idle as Mark = '1'
    TACCTL1 = SCS + CM1 + CAP + CCIE;       // Sync, Neg Edge, Capture, Int
    TACTL = TASSEL_2 + MC_2;                // SMCLK, start in continuous mode
}

//-----------------------------------------------------------------------------------
// Outputs one byte using the Timer_A UART (Tek Byte Gönderme Fonksiyonumuz)
//-----------------------------------------------------------------------------------

void SendByte(unsigned char byte)
{
    while (TACCTL0 & CCIE);                 // Ensure last char got TX'd
    TACCR0 = TAR;                           // Current state of TA counter
    TACCR0 += UART_TBIT;                    // One bit time till first bit
    TACCTL0 = OUTMOD0 + CCIE;               // Set TXD on EQU0, Int
    txData = byte;                          // Load global variable
    txData |= 0x100;                        // Add mark stop bit to TXData
    txData <<= 1;                           // Add space start bit
}

//-----------------------------------------------------------------------------------
// Receives one byte using the Timer_A UART (Tek byte Alma Fonksiyonumuz)
//-----------------------------------------------------------------------------------

unsigned char RecvByte()
{
    // Gelen karakteri bekle
    __bis_SR_register(LPM0_bits);//Veri gelene kadar CPU'yu kapat(Düþük güç için)
    return rxBuffer;
}

//-----------------------------------------------------------------------------------
// Receives String using the Timer_A UART  (String Alma Fonksiyonumuz)
//-----------------------------------------------------------------------------------

void RecvStr(char *msg, unsigned char n)
{
    register unsigned int i;
    for(i = 0; i < n - 2; i++){
    	__bis_SR_register(LPM0_bits);//Veri gelene kadar CPU'yu kapat(Düþük güç için)
   		*(msg + i) = rxBuffer;       //Karakterleri deðiþkene al
   		if (rxBuffer == '\n') //Bitirme þartý gerçekleþtiðinde döngüden çýk
   			break;
    }
   	*(msg + (i + 1)) = '\0';		//Satýr sonu ekle
}
 
//-----------------------------------------------------------------------------------
// Temp string (Gereksiz Veri Fonksiyonumuz)
//-----------------------------------------------------------------------------------

void RecvStrTEMP(void)
{
    for (;;)
    {
    	__bis_SR_register(LPM0_bits);
   		if (rxBuffer == '\n')
   			break;
    }
}
 
//-----------------------------------------------------------------------------------
// Prints a string over using the Timer_A UART (String Gönderme Fonksiyonumuz)
//-----------------------------------------------------------------------------------

void SendStr(char *msg)
{
	while (*msg)
       	SendByte(*msg++);
}

//-----------------------------------------------------------------------------------
// Timer_A UART - Transmit Interrupt Handler (Gönderme Kesme Vektörü)
//-----------------------------------------------------------------------------------

#pragma vector = TIMERA0_VECTOR
__interrupt void Timer_A0_ISR(void)
{
    static unsigned char txBitCnt = 10; 
 
    TACCR0 += UART_TBIT;                    // Add Offset to CCRx
    if (txBitCnt == 0) {                    // All bits TXed?
        TACCTL0 &= ~CCIE;                   // All bits TXed, disable interrupt
        txBitCnt = 10;                      // Re-load bit counter
    }
    else {
        if (txData & 0x01) {
          TACCTL0 &= ~OUTMOD2;              // TX Mark '1'
        }
        else {
          TACCTL0 |= OUTMOD2;               // TX Space '0'
        }
        txData >>= 1;
        txBitCnt--;
    }
}

//-----------------------------------------------------------------------------------
// Timer_A UART - Receive Interrupt Handler (Alma Kesme Vektörü)
//-----------------------------------------------------------------------------------

#pragma vector = TIMERA1_VECTOR
__interrupt void Timer_A1_ISR(void)
{
    static unsigned char rxBitCnt = 8;
    static unsigned char rxData = 0; 
 
    switch (__even_in_range(TAIV, TAIV_TAIFG)) { // Use calculated branching
        case TAIV_TACCR1:                        // TACCR1 CCIFG - UART RX
            TACCR1 += UART_TBIT;                 // Add Offset to CCRx
            if (TACCTL1 & CAP) {                 // Capture mode = start bit edge
                TACCTL1 &= ~CAP;                 // Switch capture to compare mode
                TACCR1 += UART_TBIT_DIV_2;       // Point CCRx to middle of D0
            }
            else {
                rxData >>= 1;
                if (TACCTL1 & SCCI) {            // Get bit waiting in receive latch
                    rxData |= 0x80;
                }
                rxBitCnt--;
                if (rxBitCnt == 0) {             // All bits RXed?
                    rxBuffer = rxData;           // Store in global variable
                    rxBitCnt = 8;                // Re-load bit counter
                    TACCTL1 |= CAP;              // Switch compare to capture mode
                    __bic_SR_register_on_exit(LPM0_bits);  // Clear LPM0 bits from 0(SR)
                }
            }
            break;
    }
}

#define CS 83

void main(void)
{
            //char msg[CS];  // En fazla (CS-1) karakter alýr.
 
    WDTCTL = WDTPW + WDTHOLD; 	// Stop watchdog timer (Watchdog Timer Kapatýldý)
 
    DCOCTL = 0x00;         	// Set DCOCLK to 1MHz (Frekans 1MHZ olarak ayarlandý)
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ; 
 
    P1OUT = 0x00;                   // Initialize all GPIO
    P1SEL |= UART_TXD + UART_RXD;   // Timer function for TXD/RXD pins
    P1DIR |= 0xFF & ~UART_RXD;      // Set all pins but RXD to output 
    TACCR0=400;
    TACCTL0=CCIE;
    TACTL=MC_1|ID_3|TASSEL_2;
 
    __enable_interrupt(); 			//Kesmeleri aktif ettik
	char ch[CS];
 
	uart();	//Temel uart konfigürasyonu çaðrýsý...
	RecvStrTEMP(); //Ýlk olarak 2 satýr Modul versiyon bilgisi gelir ve
	RecvStrTEMP(); //proje için çöp veridir.
 
	for(;;)
	{
 
		RecvStr(ch,CS);	//$GPGGA anahtarlý veri alýnýyor.
		RecvStrTEMP();	//Gereksiz bilgi...
		RecvStrTEMP();	//Gereksiz bilgi...
		RecvStrTEMP();	//Gereksiz bilgi...
		SendStr(ch);	//Okunan veri gönderiliyor.
 
	}
}