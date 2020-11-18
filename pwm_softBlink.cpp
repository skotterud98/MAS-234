#define F_CPU 1000000UL   //1 MHz
#include <util/delay.h>
#include <avr/io.h>

int dc;                 //duty cycle
void pwm (int dc);      //PWM function with dutycycle as input
int wave;               //Variable to generate "saw tooth" wave to compare with duty cycle

int main()
{
    DDRC |= 1 << PC5;       //Set PC5 as output
    
    while(1)
    {
        for (int i = 0; i < 100; i++) //Increment duty cycle
        {
            pwm(i);
            _delay_ms(10);
            
            if(i == 99)
            {
                for(int i = 99; i > 0; i--)     //Decrement duty cycle
                {
                    pwm(i);
                    _delay_ms(10);
                }
            }
        }
    }
}

void pwm(int dc)
{
    wave = 0;
    for(int k = 0; k < 100; k++) //Increment k up to 100
    {
        wave++;         //Generate "saw tooth" wave
        if(wave < dc)   //Compare duty cycle
        {
        PORTC |= 1 << PC5;      //Turn LED on
        }
        else
        {
        PORTC &= ~(1 << PC5);   //Turn LED off
        }
        _delay_us(10);
    }
}
