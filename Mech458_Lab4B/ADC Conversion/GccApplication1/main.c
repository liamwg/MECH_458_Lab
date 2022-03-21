/* 	
	Course		: UVic Mechatronics 458
	Milestone	: 4B
	Title		: Interfacing with DC Motor

	Name 1:	Liam Welgan-Gunn		Student ID: V00889715
	Name 2:	Marcus Lau				Student ID: V00884155
	
	Description: This program interfaces with a DC motor and does ADC conversion.
*/


#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd.h"

// define the global variables that can be used in every function  ==========
volatile unsigned char ADC_result;
volatile unsigned int ADC_result_flag;
volatile unsigned int current_cw = 1;


void mTimer (int count)
{
   /***
      Setup Timer1 as a ms timer
	  Using polling method not Interrupt Driven
   ***/
	  
   int i;

   i = 0;

   TCCR1B |= _BV (CS11);  // Set prescaler (/8) clock 16MHz/8 -> 2MHz
   /* Set the Waveform gen. mode bit description to clear
     on compare mode only */
   TCCR1B |= _BV(WGM12);

   /* Set output compare register for 2000 cycles, 1ms */
   OCR1A = 0x07D0;
 
   /* Initialize Timer1 to zero */
   TCNT1 = 0x0000;

   /* Enable the output compare interrupt */
   //TIMSK1 |= _BV(OCIE1A);  //remove if global interrups is set (sei())

   /* Clear the Timer1 interrupt flag and begin timing */
   TIFR1 |= _BV(OCF1A);

   /* Poll the timer to determine when the timer has reached 1ms */
   while (i < count)
   {
      while ((TIFR1 & 0x02) != 0x02);
	
	   /* Clear the interrupt flag by WRITING a ONE to the bit */
	   TIFR1 |= _BV(OCF1A);
	   i++;
   } /* while */
   TCCR1B &= ~_BV (CS11);  //  disable prescalar, no Clock
   return;
}  /* mTimer */

void CW(){
	PORTB = 0b00111000;
	current_cw = 1;
}

void CCW(){
	PORTB = 0b01110000;
	current_cw = 0;
}

void brake(){
	PORTB = 0b01111000;
}

void PWM(){
TCCR0A = 0b10000011; //set first two bits to 10, clear OC0A on compare match, set last two bits to WGM mode 3, bits 011 (bits are separated into 2 registers)

TCCR0B = 0b00000011; // set bit 3 to WGM mode 3 (first bit of 011), set last 3 bits to 011 to prescale clock by 64

//TIMSK0 = 0b00000010; // set bit 1 to 1, enables OCIE0A

OCR0A = ADC_result; // set OCRA to potentiometer
}


void main()
{
	//TIMER CODE
	CLKPR = 0x80;
	CLKPR = 0x01;
	//TCCR1B |= _BV(CS11);

	cli();  // disable all of the interrupt ==========================
	
	// config the external interrupt ======================================
	EIMSK |= (_BV(INT2));      // enable INT2
	EICRA |= (_BV(ISC21) | _BV(ISC20));   // rising edge interrupt
	
	//interrupt for switching belt direction
	EIMSK |= (_BV(INT3));	//enable INT3
	EICRA |= (_BV(ISC31));// | _BV(ISC30)); //rising edge interrupt
	// config ADC =========================================================
	// by default, the ADC input (analog input is set to be ADC0 / PORTF0
	ADCSRA |= _BV(ADEN);   // enable ADC
	ADCSRA |= _BV(ADIE);   // enable interrupt of ADC
	ADMUX |= _BV(ADLAR) | _BV(REFS0);  // sets voltage reference selection and ADC left adjust result
	
	// set the PORTC as output to display the ADC result ==================
	DDRC  = 0xff;
	DDRB = 0xff;
	DDRL = 0xff; //set PORTB as output for DC motor
	// sets the Global Enable for all interrupts ==========================
	sei();

	
	//Initialize LCD module
	InitLCD(LS_BLINK|LS_ULINE);
	
	
	PWM(); //initialize PWM
	//CW(); // turn cw
	//mTimer(10);
	
		// initialize the ADC, start one conversion at the beginning ==========
		ADCSRA |= _BV(ADSC);
	
	while (1)
	{  
		PORTL = 0x10;
		//mTimer(500);
		if (ADC_result_flag) 
		{
					OCR0A = ADC_result; //change PWM to current potentiometer input
					ADC_result_flag = 0x00; //clear ADC flag
					ADCSRA |= _BV(ADSC); //start another ADC
			PORTL = 0x30;
			//Clear the screen
			/*LCDClear();
			LCDWriteStringXY(0,0,"PWM= ");
			LCDWriteIntXY(5,0,(ADC_result/2.55), 3); // display PWM %
			LCDWriteStringXY(8,0,"%");
			LCDWriteIntXY(10,0,ADC_result,3);
			LCDWriteStringXY(0,1, "Direction: "); */
			
			/*if(current_cw == 1){
				LCDWriteStringXY(11,1,"CW");
			}
			else{
				LCDWriteStringXY(11,1,"CCW");
			} */

	
			//mTimer(500);
		}
					
	}// end while
}   // end main

// sensor switch:  Active HIGH starts AD converstion =======
ISR(INT2_vect)
{  // when there is a rising edge, we need to do ADC =====================
	if((PIND & 0x04) == 0x04){
		PORTB = 0x00;
		LCDClear();
		LCDWriteStringXY(0,0, "Kill Switch Activated");
		while (1){} //infinite loop
 } 
 }  // the interrupt will be triggered if the ADC is done ======================== 
 
 ISR(ADC_vect)  {
	ADC_result = ADCH;
	ADC_result_flag = 1;
 }

ISR(INT3_vect){
	if ((PIND & 0x08) == 0x00){
	//PORTL = 0x80;	
	brake();
	mTimer(20); //debounce
	if(current_cw == 1){
			PORTB = 0b00111000;
			current_cw = 1;
	}
	else {
			PORTB = 0b00111000;
			current_cw = 1;
	}
	while((PIND & 0x08) == 0x00){
		
	}
	mTimer(20);//debounce
	//PORTL = 0x00;
	}
	
}




