#define F_CPU 1000000UL   //1 MHz
#include <util/delay.h>   //Delay library
#include <avr/io.h>       //AVR library

int main()
{
    DDRC |= 1 << PC5;       //Set PC5 as output
    
    while(1)
    {
        PORTC |= 1 << PC5;  //Set pin HIGH, LED turn on
        
        _delay_ms(100)
        
        PORTC &= ~(1 << PC5);  //Set pin LOW, LED turn off
    }
}
