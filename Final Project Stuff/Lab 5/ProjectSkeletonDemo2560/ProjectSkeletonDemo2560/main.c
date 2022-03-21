// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!
// READ EVERY SINGLE WORD IN THIS PIECE OF CODE...IF YOU DON'T YOU WILL NOT UNDERSTAND THIS!!!!!!!

// Open up the document in START -> WinAVR -> AVR LibC -> User Manual -> avr/interrupt.h
// Chapter 15, in Full Manual... THIS HAS A LOT OF IMPORTANT INFO...I have mentioned this at least 3 times!!!

// For those that are still having major problems, I've seen about 1/3 of the class with major problems in
// code structure. If you are still having major problems with your code, it's time to do a VERY quick overhaul.
// I've provided a skeleton structure with an example using two input capture interrupts on PORTDA0 and A3
// Please try this in the debugger.

// Create a watch variable on STATE. To do this right click on the variable STATE and then
// Add Watch 'STATE'. You can see how the variable changes as you click on PINDA0 or PINDA3. Note that the interrupt
// catches a rising edge. You modify this to suit your needs.

#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "lcd.h"


//Function Declarations

void mTimer(int count);

// Global Variable
volatile char STATE;
volatile unsigned int i=0;


int main(int argc, char *argv[]){

	//Timer Code
	CLKPR = 0x80;
	CLKPR = 0x01;		//  sets system clock to 8MHz
	TCCR1B |= _BV(CS11);
	
	//Initialize LCD module
	InitLCD(LS_BLINK|LS_ULINE);

	STATE = 0;

	cli();		// Disables all interrupts
	
	//Setting inputs and outputs
	DDRD = 0b11110000;	// Going to set up INT2 & INT3 on PORTD
	DDRC = 0xFF;		// just use as a display


	// Set up the Interrupt 0,3 options
	//External Interrupt Control Register A - EICRA (pg 110 and under the EXT_INT tab to the right
	// Set Interrupt sense control to catch a rising edge
	
	EIMSK |= _BV(INT2); //enable INT2
	EICRA |= _BV(ISC21) | _BV(ISC20); //rising edge interrupt
	
	EIMSK |= _BV(INT3); //enable INT3
	EICRA |= _BV(ISC31); //falling edge interrupt

		//External interupt control register
	//	EICRA &= ~_BV(ISC21) & ~_BV(ISC20); /* These lines would undo the above two lines */
	//	EICRA &= ~_BV(ISC31) & ~_BV(ISC30); /* Nice little trick */


	// See page 112 - EIFR External Interrupt Flags...notice how they reset on their own in 'C'...not in assembly
	//EIMSK |= 0x0C;

	// Enable all interrupts
	sei();	// Note this sets the Global Enable for all interrupts
	
	goto POLLING_STAGE;

	// POLLING STATE
	POLLING_STAGE:
	PORTC |= 0xF0;	// Indicates this state is active
	
	switch(STATE){
		case (0) :
		goto POLLING_STAGE;
		break;	//not needed but syntax is correct
		case (1) :
		goto MAGNETIC_STAGE;
		break;
		case (2) :
		goto REFLECTIVE_STAGE;
		break;
		case (3) :
		goto BUCKET_STAGE;
		break;
		case (5) :
		goto END;
		default :
		goto POLLING_STAGE;
	}//switch STATE
	

	MAGNETIC_STAGE:
	// Do whatever is necessary HERE
	PORTC = 0x01; // Just output pretty lights know you made it here
	//Reset the state variable
	STATE = 0;
	goto POLLING_STAGE;

	REFLECTIVE_STAGE:
	// Do whatever is necessary HERE
	PORTC = 0x04; // Just output pretty lights know you made it here
	//Reset the state variable
	STATE = 0;
	goto POLLING_STAGE;
	
	BUCKET_STAGE:
	// Do whatever is necessary HERE
	PORTC = 0x08;
	//Reset the state variable
	STATE = 0;
	goto POLLING_STAGE;
	
	END:
	// The closing STATE ... how would you get here?
	PORTC = 0xF0;	// Indicates this state is active
	// Stop everything here...'MAKE SAFE'
	return(0);

}

/* Set up the External Interrupt 2 Vector */
ISR(INT2_vect){
	mTimer(20);//debounce
	/* Toggle PORTC bit 2 */
	if((PIND & 0x04) == 0x04){
		STATE = 2;
		i++;
		LCDClear();
		LCDWriteStringXY(0,0,"Counter=");
		LCDWriteIntXY(9,0,i,5);
		mTimer(50);
	}
	
}

ISR(INT3_vect){
	mTimer(20); //debounce
	/* Toggle PORTC bit 3 */
	if((PIND & 0x08) == 0x00){ //active low, checks if switch was pressed
		STATE = 3;
		i--;
		LCDClear();
		LCDWriteStringXY(0,0,"Counter=");
		LCDWriteIntXY(9,0,i,5);
		mTimer(50);
	}
}

// If an unexpected interrupt occurs (interrupt is enabled and no handler is installed,
// which usually indicates a bug), then the default action is to reset the device by jumping
// to the reset vector. You can override this by supplying a function named BADISR_vect which
// should be defined with ISR() as such. (The name BADISR_vect is actually an alias for __vector_default.
// The latter must be used inside assembly code in case <avr/interrupt.h> is not included.
ISR(BADISR_vect)
{
	// user code here
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
	
	//TIMSK1 = TIMSK1 | 0b00000010; //Enable the output compare interrupt (removed because we are using global interrupts)
	
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

//use pina7 for hall effect sensor
//have to reconfigure adc code (acquisition)
//dont use auto trigger mode
//only do adc when part is in front of sensor (start when it enters and stop when it leaves)


