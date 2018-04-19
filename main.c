#include <msp430.h>
#define   Num_of_Results   8

volatile unsigned int A0results[Num_of_Results];

int main(void)
{
	volatile unsigned int i;
	WDTCTL = WDTPW + WDTHOLD;

	P6SEL |= BIT3;                            // P6.1 ADC option select
	ADC12CTL0 = ADC12SHT0_4 + ADC12ON;         // Sampling time, ADC12 on
	ADC12CTL1 = ADC12SHP + ADC12DIV_7;                     // Use sampling timer

	ADC12CTL2 = ADC12RES_0;
	ADC12MCTL0 = ADC12INCH_3;
	ADC12IE = 0x01;                             // Enable interrupt
	__delay_cycles(10000);
	ADC12CTL0 |= ADC12ENC;

	P4DIR |= 0xff;
	P4OUT &= ~0xff;
	P6DIR |= BIT6;
	P5DIR |= BIT0 + BIT1 + BIT6;
	P5OUT |= BIT6;
	P6OUT &= ~BIT6;
	P5OUT &= ~(BIT0 + BIT1);
	P2DIR |= BIT7;
	P2OUT &= ~BIT7;
	while(1)
	{
		ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
		__bis_SR_register(GIE);       // Enter LPM0, Enable interrupts
	}
}
//
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
	switch(__even_in_range(ADC12IV,34))
	{
		case  0: break;                           // Vector  0:  No interrupt
		case  2: break;                           // Vector  2:  ADC overflow
		case  4: break;                           // Vector  4:  ADC timing overflow
		case  6:                                  // Vector  6:  ADC12IFG0
			if(ADC12MEM0>128)
				P2OUT |= BIT7;
			else
				P2OUT &= ~BIT7;

			P4OUT = ADC12MEM0;
			 __no_operation();
		case  8: break;                           // Vector  8:  ADC12IFG1
		case 10: break;                           // Vector 10:  ADC12IFG2
		case 12: break;                           // Vector 12:  ADC12IFG3
		case 14: break;                           // Vector 14:  ADC12IFG4
		case 16: break;                           // Vector 16:  ADC12IFG5
		case 18: break;                           // Vector 18:  ADC12IFG6
		case 20: break;                           // Vector 20:  ADC12IFG7
		case 22: break;                           // Vector 22:  ADC12IFG8
		case 24: break;                           // Vector 24:  ADC12IFG9
		case 26: break;                           // Vector 26:  ADC12IFG10
		case 28: break;                           // Vector 28:  ADC12IFG11
		case 30: break;                           // Vector 30:  ADC12IFG12
		case 32: break;                           // Vector 32:  ADC12IFG13
		case 34: break;                           // Vector 34:  ADC12IFG14
		default: break;
	}
}
