/* ##################################################################
# MILESTONE: 2
# PROGRAM: 1
# PROJECT: Lab2
# GROUP: 1
# NAME 1: Liam, Welgan-Gunn, V00889715
# NAME 2: Marcus, Lau, V00884155
# DESC: This turns on LEDs in a Knight Rider pattern
# DATA
# REVISED ############################################################### */

#include <stdlib.h> // the header of the general-purpose standard library of C programming language
#include <avr/io.h>// the header of I/O port
#include <util/delay_basic.h>
#include <avr/interrupt.h> //Needed for interrupts

void mTimer(int count);

/* ################## MAIN ROUTINE ################## */
int main(int argc, char *argv[]){
	
	//TIMER CODE
	CLKPR = 0x80;
	CLKPR = 0x01;
	
	//Timer Instructions
	//Sets timer 1 to run at 1Mz. note CPU clock set to 8MHz
	
	TCCR1B |= _BV(CS11);	//BV sets the bit to logic 1
						//Note the register is TCCR1B1
						//TCCR1 is the timer/counter control resister 1
						//B is the "B" register and 1 is bit 1
						//CS means clock select, has the pre-scaler set to 8
						
	DDRL = 0xFF;
		
/*	
	while(1){
		
		PORTL = 0b01000000;
		mTimer(1000); //left GREEN
		
		PORTL = 0b00010000;
		mTimer(1000); //right YELLOW
		
	}
*/
	
	//Knight Rider
	
	DDRC = 0xFF;
	
	
	while(1){
		PORTC = 0b11000000;
		mTimer(200);
		
		for(int k = 1; k < 7; k++){
			PORTC = PORTC >> 1;
			mTimer(200);
		}
		
		for(int k = 1; k < 6; k++){
			PORTC = PORTC << 1;
			mTimer(200);
		}	
		
	}
}
	
void mTimer(int count){
	/* this system clock is 8MHz, the timer runs on the CPU clock which is a function of the system clock.
	The system clock has been pre-scaled by 2. 
	*/
	
	int i; //keeps track of loop number
	
	i = 0; //initializes loop counter
	
	//Set the Waveform Generation Mode bit description to Clear Timer on Compare Math mode (CTC) only
	
	TCCR1B |=_BV(WGM12);		//Set WGM bits to 0100, page 145
							//note WGM is spread over two register
							
	OCR1A = 0x03E8; //Sets output compare register for 1000 cycles = 1ms
	
	TCNT1 = 0x0000; //Sets initial value of timer counter to 0x0000
	
	TIMSK1 = TIMSK1 | 0b00000010; //Enable the output compare interrupt
	
	TIFR1 |= _BV(OCF1A); //clear timer interrupt flag and begin new timing
	
	//Poll the timer to determine when the timer has reached 0x03E8
	
	while(i<count){
		
		if((TIFR1 & 0x02) == 0x02){
			
			TIFR1 |= _BV(OCF1A); // clear interrupt flag by writing a ONE to the bit
			
			i++; //increment loop number
						
		} // end if
	} // end while
	
	return;
	
}// mTimer	


