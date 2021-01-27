/*

	ATtiny85_VCO.c

	Author : Simon Juhl
	www.SimonJuhl.net

	Translates a voltage from 0V to 5V into
	a pitch ranging from a high tone to a low:
	1V per octave

 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

// clock and timing
#define F_CLOCK 8000000		// clock frequncy
#define TIMER_DIV 256	// timer-division
#define CV_SENS 1

// Musical scaling
// BOTTOM_TONE	= 0V
// TOP_TON		= 5V 
#define BOTTOM_TONE 220.0
#define TOP_TONE 1320.0
#define DIFF_TONE (TOP_TONE - BOTTOM_TONE)

// prototypes
void setup();
void setFreq(double);

double currentPitch;
unsigned int lastADC = 0;
double timerComp = (F_CLOCK / (TIMER_DIV));

// main
int main(void)
{
    setup();
	//setFreq(666);
    while (1){}
}

/* register setup*/
void setup()
{
	// I/O
	DDRB |= (1<<PB0) | (0<<PB2); // PB0 is output and PB2 is input
	
	// ADC (CV input)
	ADMUX = (1<<MUX0); /* Use PB2 as ADC input */
	ADCSRA = 
		(1<<ADEN) | /* enable ADC				*/
		(1<<ADIE) |	/* enable ADC interrupt		*/
		(1<<ADSC) | /* Start Conversion			*/
		(1<<ADATE)| /* Enable ADC auto-trigger	*/
		(1<<ADPS0)|(1<<ADPS1); /* ADC prescaler set to 8 */
	
	// TIMER0 used for VCO output
	TCCR0A	|=
		(1<<COM0A0)	|	/* Toggle compare output on PB0 */
		(1<<WGM01)	;	/* CTC mode */
	
	// setting prescaler to 256 for timer0
	// Frequency range is 69 Hz to 7812 Hz
	TCCR0B	|=	(1<<CS02);	
	
	sei(); // enable global interrupt
}

/*	From 69 Hz to 7812 Hz	*/
void setFreq(double pitch)
{
	OCR0A = (timerComp/pitch)-1;
}

/* ADC conversion interrupt */
ISR(ADC_vect)
{
	currentPitch = ((double)ADC * (DIFF_TONE) / 1024.0 + BOTTOM_TONE) ; // range ADC to 1V/Octave
	setFreq(currentPitch); // set frequency
	lastADC = ADC;
}


