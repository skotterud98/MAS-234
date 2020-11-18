#include <FlexCAN.h>   //Library to be able to use the CAN trancievers connected to the teensy

CAN_message_t rx_msg;        //Creating CAN bus message objects
CAN_message_t tx_msg;

void setup()
{
    Can0.begin(250000);   //Activate Can0 object with baudrate set to 250 kbit/s
}
void loop()
{
  Can0.read(rx_msg);  //Read CAN message
  
  tx_msg.len = 1;     //Number of transmit buffers
  tx_msg.id = 420;    //New message identifier, different from last
  tx_msg.buf[0] = rx_msg.buf[0] / 10;   //Divide potmeter value with 10
  
  Can0.write(tx_msg);   //Send message frame

  delay(200);
}
