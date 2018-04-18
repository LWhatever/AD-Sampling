#include <msp430.h>

char * Flash_ptr = (char *) 0x002400;
char * Flash_ptr0 = (char *) 0x002400;
volatile int write_mode = -1;

void flash_write()
{
	FCTL3 = FWKEY;                            // Clear Lock bit
	FCTL1 = FWKEY+ERASE;                      // Set Erase bit
	FCTL1 = FWKEY+WRT;
	if(write_mode==1)
		*Flash_ptr++ = ADC12MEM0;
	if(Flash_ptr == (char *) 0x0043FF)
		write_mode = 0;
	FCTL1 = FWKEY;                            // Clear WRT bit
	FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
}

void GPIO_Init()
{
	P4DIR |= 0xff;
	P4OUT &= ~0xff;
	P6DIR |= BIT6;
	P5DIR |= BIT0 + BIT1 + BIT6;
	P5OUT |= BIT6;
	P6OUT &= ~BIT6;
	P5OUT &= ~(BIT0 + BIT1);
	P2DIR |= BIT7;
	P2OUT &= ~BIT7;
	P8DIR |= BIT0;
	P8OUT &= ~BIT0;

	P3DIR |= BIT2;
	P3OUT &= ~BIT2;


	P2REN |= BIT6;
	P2OUT |= BIT6;
}

int main(void)
{
	volatile unsigned int i;
	WDTCTL = WDTPW + WDTHOLD;
	P6SEL |= BIT3;                            // P6.1 ADC option select
	ADC12CTL0 = ADC12ON + ADC12MSC;         // Sampling time, ADC12 on
	ADC12CTL1 = ADC12CONSEQ1 + ADC12SHP;
	ADC12CTL2 = ADC12RES_0;
	ADC12MCTL0 = ADC12INCH_3;
	ADC12CTL0 |= ADC12ENC;


	TA0CTL |= MC_1 + TASSEL_2 + TACLR;
	TA0CCTL0 = CCIE; //比较器中断使能
	TA0CCR0 = 125; //比较值设为4096，相当于1s的时间间隔

	TA1CCTL1 = CCIE;                          // CCR0 interrupt enabled
	TA1CCR0 = 10000;
	TA1CTL = TASSEL_1 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR

	GPIO_Init();
	__bis_SR_register(GIE);

	while(1)
	{
		ADC12CTL0 |= ADC12SC;      // Start sampling/conversion


	}
}
//
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
	P3OUT ^= BIT2;
	if(write_mode == -1)
		P4OUT = ADC12MEM0;
	flash_write();
}


#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
	if(!(P2IN & BIT6))
	{
		__delay_cycles(100000);
		P8OUT ^= BIT0;
		write_mode = 1;
		Flash_ptr0 = (char *) 0x002400;
		Flash_ptr = (char *) 0x002400;
	}
	if(write_mode == 0)
	{
		P4OUT = *Flash_ptr0++;
		if(Flash_ptr0 == Flash_ptr)
		{
			write_mode == -1;
			Flash_ptr0 = (char *) 0x002400;
			Flash_ptr = (char *) 0x002400;
		}
	}
}
