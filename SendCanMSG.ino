#include <FlexCAN.h>   //Library to be able to use the CAN trancievers connected to the teensy

CAN_message_t msg;        //Creating CAN bus message objects

void setup()
{
    Can0.begin(250000);   //Activate Can0 and Can1 object with baudrate set to 250 kbit/s
    Serial.begin(9600);   //Start up serial monitor
}
void loop()
{
    if(Can0.available())      //Check if a CAN message is available
    {
        Can0.read(msg);    //Read the recieved CAN message

        Serial.print("Message ID: ");
        Serial.print(msg.id);        //Print message id to serial monitor
        Serial.print("\t\t");
        
        for(int i = 0; i < rx_msg.len; i++)   //Go through all potentional transmit buffers in frame
        {
            Serial.print("buf[");
            Serial.print(i);
            Serial.print("] = ");
            Serial.print(msg.buf[i]);    //Print transmit buffers to serial monitor
            Serial.print("\t");
        }

        Serial.println();   //Lineshift

        Can0.write(msg);   //Return the same CAN message back
    }
}
