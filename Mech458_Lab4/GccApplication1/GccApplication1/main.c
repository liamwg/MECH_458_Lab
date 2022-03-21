/* 	
	Course		: UVic Mechatronics 458
	Milestone	: 4A
	Title		: Interfacing with stepper motor

	Name 1:	Liam Welgan-Gunn		Student ID: V00889715
	Name 2:	Marcus Lau				Student ID: V00884155
	
	Description: This program interfaces with a stepper motor. It turns the motor in both CW and CCW in relative increments of 30, 60, 180 degrees.
*/

#include <stdlib.h>
#include <avr/io.h>

volatile int position;
void CW(int steps);
void CCW(int steps);
void mTimer(int count);

int main(){
	
	//TIMER CODE
	CLKPR = 0x80;
	CLKPR = 0x01;
	TCCR1B |= _BV(CS11);

// Set Ports A, C, F to outputs
	DDRA = 0xFF;
	DDRC = 0xFF;
	DDRF = 0xFF;
	DDRB = 0xFF;
	
//PWM code
	TCCR0A = 0b10000011; //set first two bits to 10, clear OC0A on compare match, set last two bits to WGM mode 3, bits 011 (bits are separated into 2 registers)
	
	TCCR0B = 0b00000011; // set bit 3 to WGM mode 3 (first bit of 011), set last 3 bits to 011 to prescale clock by 64
	
	TIMSK0 = 0b00000010; // set bit 1 to 1, enables OCIE0A
	
	OCR0A = 0x80; // set OCRA to 50% duty cycle (128)
	
	
	while(1){
		CW(16);
		mTimer(1000);
		CW(32);
		mTimer(1000);
		CW(100);
		mTimer(1000);
		CCW(16);
		mTimer(1000);
		CCW(32);
		mTimer(1000);
		CCW(100);
		mTimer(1000);
		
	}
	
}

void CW (int steps){
	
		char step1 = 0b00110000;
		char step2 = 0b00000110;
		char step3 = 0b00101000;
		char step4 = 0b00000101;
		
	for (int k = 0; k < (steps/4); k++){
		int step_wait = 20;
		PORTA = step1;
		mTimer(step_wait);
		PORTA = step2;
		mTimer(step_wait);
		PORTA = step3;
		mTimer(step_wait);
		PORTA = step4;
		mTimer(step_wait);
	}	
}

void CCW(int steps){
	
	char step1 = 0b00110000;
	char step2 = 0b00000110;
	char step3 = 0b00101000;
	char step4 = 0b00000101;
	
	for (int k = 0; k < (steps/4); k++){
		int step_wait = 20;
		PORTA = step4;
		mTimer(step_wait);
		PORTA = step3;
		mTimer(step_wait);
		PORTA = step2;
		mTimer(step_wait);
		PORTA = step1;
		mTimer(step_wait);
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

