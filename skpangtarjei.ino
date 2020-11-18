#include <FlexCAN.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const byte OLED_DC      = 6;
const byte OLED_CS      = 10;   //OLED pins
const byte OLED_RESET   = 5;
Adafruit_SSD1306 myOLED(OLED_DC, OLED_RESET, OLED_CS);    //Creating OLED object

CAN_message_t tx_msg;        //Creating CAN bus message objects
CAN_message_t rx_msg;

int countRead;      //Counter variable for number of read messages
int countWrite;     //Counter variable for number of sent messages
int id;             //Message ID variable

void setup()
{
    Can0.begin(250000);   //Activate Can0 and Can1 object with baudrate set to 250 kbit/s
    Can1.begin(250000);

    tx_msg.len = 8;          //Length of message
    tx_msg.id = 0x1A4;       //Message ID, 420d
    tx_msg.buf[0] = 12;      //Transmit buffer

    myOLED.begin(SSD1306_SWITCHCAPVCC);   //Start up OLED object
    
}
void loop()
{
    Can0.write(tx_msg);        //Transmit tx_msg message on Can0
    Can1.read(rx_msg);      //Recieve message on Can1

    id = rx_msg.id;         //Store recieved message ID in id variable

    if(Can0.write(tx_msg))     //Returns true if a message is transmitted
    {
        countWrite++;               //Counting transmitted CAN bus messages
        
        if (countWrite>100000)
        {
          countWrite = 0;           //Limiting max counted messages to start at zero
        }
    }
    if(Can1.read(rx_msg))   //Returns true if a message is received
    {
        countRead++;                //Counting recieved CAN bus messages
        
        if (countRead>100000)
        {
          countRead = 0;            //Limiting max counted messages to start at zero
        }
    }
    
    myOLED.clearDisplay();
    myOLED.drawRoundRect(0, 0, 128, 64, 0, WHITE);    //Display a rectangel frame
    myOLED.setTextSize(0);
    myOLED.setTextColor(WHITE);   //Set text size and color
    myOLED.setCursor(0, 5);
    myOLED.println(" MAS-234 - Gruppe 11");
    myOLED.println(" ");
    myOLED.println(" CAN-statistikk");
    myOLED.println(" -------------------");
    myOLED.print(" Sendt:   ");
    myOLED.println(countWrite);                   //Displaying number of transmitted messages
    myOLED.print(" Mottatt: ");
    myOLED.println(countRead);                    //Displaying number of recieved messages
    myOLED.print(" Mottok sist ID: ");
    myOLED.println(id);                            //Displaying ID number of last recieved message
    myOLED.display();
}
