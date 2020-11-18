#include <FlexCAN.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

CAN_message_t tx_ball;
CAN_message_t tx_paddle;
CAN_message_t rx_msg;

const byte joyUp      = 22;   //Joystick pins
const byte joyDown    = 23;
const byte OLED_DC    = 6;    //OLED pins
const byte OLED_CS    = 10;
const byte OLED_RESET = 5;

Adafruit_SSD1306 myOLED(OLED_DC, OLED_RESET, OLED_CS);    //Creating OLED object with pins as input

unsigned long currentMillis;      //Milliseconds variable to time processes without blocking the program with delay() function
unsigned long lastMillis = 0;
const int update_rate = 10;      //Milliseconds
bool updateCAN = false;           //Logic variable to control when to transmit ball position coordinates through CAN bus

const byte myPaddle_x = 118;      //x-coordinate of my paddle
const byte yourPaddle_x = 10;     //x-coordinate of your paddle
byte myPaddle_y = 22;             //y-coordinate of my paddle
byte yourPaddle_y = 22;           //y-coordinate of your paddle

byte yourPaddle_prev = 1;        //Previous y-coordinate of your paddle
byte myPaddle_prev = 1;          //Previous y-coordinate of my paddle


byte yourScore;                   //Your total score
byte myScore;                     //My total score

byte ballX = 64;                  //Coordinates of ball, initialized in the middle of display
byte ballY = 32;                  
byte dirX = 1;                    //Direction of ball
byte dirY = 1;
int randomY;                      //Variable to make random y-direction after score

byte newBallX;                    //New position coordinates of ball
byte newBallY;

bool upState;                     //State variable for up click on joystick
bool downState;                   //State variable for down click on joystick
bool gameStart = true;            //Logic variable to tell program that it should go through start-loop
bool gameMaster = false;          //Game master or not

void goal();
void printScore();
void transmitCAN();               //Declaring functions used. Definition of them at the bottom of script
void recieveCAN();

/*****************************************************************/

void setup()
{
    Can0.begin(250000);                   //Activate automated created Can0 object with baudrate set to 125 kbit/s
    
    myOLED.begin(SSD1306_SWITCHCAPVCC);   //Activate OLED

    pinMode(joyUp, INPUT_PULLUP);
    pinMode(joyDown, INPUT_PULLUP);       //Setting joystick pins as input with pullups resistors

   
    myOLED.clearDisplay();
    myOLED.display();
    myOLED.setTextSize(0);                //Clear display and set text size and color
    myOLED.setTextColor(WHITE, BLACK);
}  


void loop()
{
    currentMillis = millis();                   //Update current time variable with the actual current time in milliseconds
    
    //Transmit ball movement over CAN
    tx_ball.len = 4;                      //Data length, 4 bytes
    tx_ball.id = 61;                      //Message ID, group number (11) + 50
    tx_ball.buf[0] = ballX;               //Coordinate data of gamemasters generated ball
    tx_ball.buf[1] = ballY;
    tx_ball.buf[2] = yourScore;           //Score data
    tx_ball.buf[3] = myScore;

    //Transmit paddle movement over CAN
    tx_paddle.len = 2;                    //Data length, 2 bytes
    tx_paddle.id = 31;                    //Message ID, group number (11) + 20
    tx_paddle.buf[0] = myPaddle_y;        //My paddle y-coordinate data
    tx_paddle.buf[1] = 1;                 //Game begin data

    
    while(gameStart)    //Deciding who is master
    {
        myOLED.setCursor(16, 31);
        myOLED.print("Press joystick...");      //Display start message until one of the players press joystick
        myOLED.display();

        Can0.read(rx_msg);
        
        if(digitalRead(joyUp) == LOW || digitalRead(joyDown) == LOW || rx_msg.buf[1] == 1)
        {
            if(rx_msg.buf[1] != 1)
            {
                gameMaster = true;
                Can0.write(tx_paddle);    //If joystick is pressed, this is now master, send CAN message to other player to act as slave.
            }
            gameStart = false;
            myOLED.clearDisplay();                  //If CAN message from the other player is received, act as slave
            myOLED.drawRect(0, 0, 128, 64, WHITE);
            myOLED.display();
        }
    }
    
    transmitCAN();    //Transmit my paddle position and ball coordinate
    recieveCAN();     //Recieve your paddle position and ball coordinate if not game master

    upState = false;    //Set joystick states to false
    downState = false;

    upState |= (digitalRead(joyUp) == LOW);
    downState |= (digitalRead(joyDown) == LOW);   //If joystick is clicked, change state to true
    
    if(currentMillis - lastMillis >= update_rate)   //Check if it is time to update positions
    {   
        if(gameMaster)      //If this is game master
        {
            updateCAN = true;     //Change state of transmit CAN variable for ball transmit
            transmitCAN();        //Send ball coordinates on CAN every 10 ms
            lastMillis = millis();
            
            //Ball
            newBallX = ballX + dirX;    //Calculate new position of ball
            newBallY = ballY + dirY;
            if(newBallX == 1 || newBallX == 127)    // Hit verticall walls
            {
                goal();     //Execute goal function
                return;
            }
            if(newBallY == 1 || newBallY == 63)     // Hit horisontal walls
            {
                dirY = -dirY;
                newBallY += dirY + dirY;    //Bounce with new y-direction
            }
            if(newBallX == myPaddle_x && newBallY >= myPaddle_y && newBallY <= myPaddle_y + 20) //Hit my paddle
            {
                dirX = -dirX;
                newBallX += dirX + dirX;    //Bounce with new y-direction
            }
            if(newBallX == yourPaddle_x && newBallY >= yourPaddle_y && newBallY <= yourPaddle_y + 20) //Hit your paddle
            {
                dirX = -dirX;
                newBallX += dirX + dirX;    //Bounce with new y-direction
            }
            myOLED.drawPixel(ballX, ballY, BLACK);
            myOLED.drawPixel(newBallX, newBallY, WHITE);    //Display the balls new postition and delete old position
            ballX = newBallX;
            ballY = newBallY;
        }


        // My paddle
        myOLED.drawFastVLine(myPaddle_x, myPaddle_y, 20, BLACK);
    
        if(upState)             //If joystick up is clicked
        {
            myPaddle_y--;       //Move paddle up
        }
        if(downState)           //If joystick down is clicked
        {
            myPaddle_y++;       //Move paddle down
        }
        upState = downState = false;      //Reset joystick states
        
        if(myPaddle_y < 1)
        {
            myPaddle_y = 1;         
        }
        if(myPaddle_y + 20 > 63)          //Limiting maximum paddle positions in both y-directions
        {
            myPaddle_y = 63 - 20;
        }
        myOLED.drawFastVLine(myPaddle_x, myPaddle_y, 20, WHITE);

        printScore();     //Print my and your score
    }
}

/*******************************************************************/

void goal()
{
    myOLED.drawPixel(ballX, ballY, BLACK);    //Remove ball
    myOLED.display();
    
    if(newBallX == 127)         //If ball hits my vertical line
    {
        yourScore++;            //Add one point to your score
        ballX = myPaddle_x - 1;
        ballY = myPaddle_y + 10;     //Initilize ball position to start at my paddle
        dirX = -1;
        
        randomY = random(0, 10); //Random generator from 0 to 9
        if(randomY < 5)
        {
            dirY = -1;
        }
        if(randomY >= 5)            //Making the y-direction random
        {
            dirY = 1;
        }
    }
    else if (newBallX == 1)     //If ball hits your vertcal line
    {
        myScore++;              //Add one point to my score
        ballX = yourPaddle_x + 1;
        ballY = yourPaddle_y + 10;     //Initilize ball position to start at your paddle
        dirX = 1;
        
        randomY = random(0, 10); //Random generator from 0 to 9
        if(randomY < 5)
        {
            dirY = -1;
        }
        if(randomY >= 5)        //Making the y-direction random
        {
            dirY = 1;
        }
    }
}


void printScore()
{
    if(gameMaster)
    {
        myOLED.setTextSize(0);
        myOLED.setTextColor(WHITE, BLACK);      //Printing my and your score in shown order, if i am game master
        myOLED.setCursor(30, 5);
        myOLED.print(yourScore);
        myOLED.display();
    
        myOLED.setTextSize(0);
        myOLED.setTextColor(WHITE, BLACK);
        myOLED.setCursor(90, 5);
        myOLED.print(myScore);
        myOLED.display();
    }
    else
    {
        myOLED.setTextSize(0);
        myOLED.setTextColor(WHITE, BLACK);      //Printing my and your score in shown order, if other player is game master
        myOLED.setCursor(30, 5);
        myOLED.print(myScore);
        myOLED.display();
    
        myOLED.setTextSize(0);
        myOLED.setTextColor(WHITE, BLACK);
        myOLED.setCursor(90, 5);
        myOLED.print(yourScore);
        myOLED.display();
    }
}


void transmitCAN()
{
    if(gameMaster && updateCAN)
    {
        Can0.write(tx_ball);          //Transmitting ball coordinates if i am game master and it is time to update (every 10th ms)
        updateCAN = false;
    }
    
    if(myPaddle_y != myPaddle_prev && !updateCAN)
    {
        Can0.write(tx_paddle);        //Transmitting my paddle y-coordinates to other player if it has changed
        myPaddle_prev = myPaddle_y;
    }
}


void recieveCAN()
{
    if(Can0.available())        //Check if there is any CAN messages available
    {
        Can0.read(rx_msg);      //Read CAN message

        if(gameMaster == false && rx_msg.id == 61)   //Other player is game master and recieved message has the ball ID
        {
            newBallX = 128 - rx_msg.buf[0];
            newBallY = rx_msg.buf[1];               //Update ball coordinates and score with the recieved data
            yourScore = rx_msg.buf[2];
            myScore = rx_msg.buf[3];
            
            if(newBallX == 1 || newBallX == 127)    //Hit verticall walls
            {
                goal();
            }
            else
            {
                myOLED.drawPixel(ballX, ballY, BLACK);
                myOLED.drawPixel(newBallX, newBallY, WHITE);    // Same as goal above but for when i am not game master
            }
            ballX = newBallX;
            ballY = newBallY;
        }
        
        if(rx_msg.id == 31)   //If read CAN message has ID of paddle data
        {
            yourPaddle_y = rx_msg.buf[0];     //Update your paddle y-coordinates with recieved data
        }
        if(yourPaddle_y != yourPaddle_prev)
        {
            myOLED.drawFastVLine(yourPaddle_x, yourPaddle_prev, 20, BLACK);   //Display your paddle if it has changed position
            myOLED.drawFastVLine(yourPaddle_x, yourPaddle_y, 20, WHITE);
        }
        yourPaddle_prev = yourPaddle_y;

        myOLED.display();   //Display changes
    }
}
