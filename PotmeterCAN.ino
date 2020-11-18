#include <FlexCAN.h>   //Library to be able to use the CAN trancievers connected to the teensy

CAN_message_t tx_msg;        //Creating CAN bus message objects
int potValue;

void setup()
{
    Can0.begin(250000);   //Activate Can0 object with baudrate set to 250 kbit/s
    Serial.begin(9600);   //Start up serial monitor

    pinMode(A6, INPUT);   //Set pin as input
}
void loop()
{
    potValue = analogRead(A6);    //Read pometer value
    
    tx_msg.len = 1;   //Number of transmit buffers
    tx_msg.id = 430;  //Message identifier
    tx_msg.buf[0] = map(potValue, 1, 1020, 0, 255);   //Map potmeter value to suitable byte range

    Can0.write(tx_msg);   //Send message fram

    Serial.println(tx_msg.buf[0]);  //Print transmit buffer to serial monitor

    delay(194);
}
