//Author: Thomas Pugh (16021460)

#include "MicroBit.h"

#define BIT_TIME 20

struct Point
{
    int x;
    int y;
};


MicroBit uBit;
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_ALL);
MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_ALL);
int binary[3];
int x_value;

//Encryption Stuff
int sendIndex = 0;
int receiveIndex = 0;
ManagedString password("Password123"); // Worlds strongest password, 10% of the time it works 100% of the time.

//Creates image same size as screen
MicroBitImage shootout(5,5);
Point player;
Point bullet;
Point incomingBullet;
int PLAYER_SPEED = 150;
int BULLET_SPEED = 100;
int game_over;
int playerScore = 0;

void incomingFire(int bulletX)
{
    if (incomingBullet.y == -1)
    {
        incomingBullet.x = bulletX;
        incomingBullet.y = 0;
    }
}

uint8_t getBits() {
    // Tell server we'd like some data. Pretty please. :-)
    uBit.io.P1.setDigitalValue(1);
    uBit.sleep(BIT_TIME);
    uBit.io.P1.setDigitalValue(0);
    uBit.io.P1.getDigitalValue(); // Let DAL know we're now fetching.
    uBit.sleep(BIT_TIME);

    uint8_t bits = 0;

    // Retrieve the good stuff.
    for (int i = 0; i < 3; i++) {
        int bit = uBit.io.P1.getDigitalValue();
        bits <<= 1;
        bits += bit;
        uBit.sleep(BIT_TIME);
    }

    // XOR Decryption.
    /*receiveIndex += 1;
    receiveIndex %= password.length();
    bits ^= password.toCharArray()[receiveIndex];
    bits &= 0x7;*/

    return bits;
}

void sendBits(uint8_t bits) {
    // Wait until this data has been requested.
    while (uBit.io.P0.getDigitalValue() == 0) {
        uBit.sleep(BIT_TIME);
    }

    // Allow client micro:bit to update status to read.
    uBit.sleep(BIT_TIME);

    // XOR Encryption.
    /*sendIndex += 1;
    sendIndex %= password.length();
    bits ^= password.toCharArray()[sendIndex];*/

    // Send the good stuff.
    for (int i = 2; i >= 0; i--) {
        int bit = (bits >> i) & 0x01;
        uBit.io.P0.setDigitalValue(bit);
        uBit.sleep(BIT_TIME);
    }
}

//Changes player position based on tilt of accelerometer
void playerUpdate()
{
    while(!game_over)
    {
        uBit.sleep(PLAYER_SPEED);
        if(uBit.accelerometer.getX() < -300 && player.x > 0)
            player.x--;
        if(uBit.accelerometer.getX() > 300 && player.x < 4)
            player.x++;
    }
}

void incomingBulletUpdate() {
    while (!game_over)
    {
        uBit.sleep(BULLET_SPEED);
        if (incomingBullet.y != -1)
        {
            incomingBullet.y++;
        }

        //If bullet goes out of bounds it is reset until fire is triggered again
        if (shootout.getPixelValue(incomingBullet.x, incomingBullet.y) > 0)
        {
            shootout.setPixelValue(incomingBullet.x, incomingBullet.y, 0);
            incomingBullet.x = -1;
            incomingBullet.y = -1;
        }
    }
}

//Checks location of bullet, changes it to location of player
//Triggered by pressing button A
void fire(MicroBitEvent)
{
    if (bullet.y == -1)
    {
        bullet.y = 4;
        bullet.x = player.x;
    }
}

//Updates location of bullet, starts at location of player due to fire
void bulletUpdate()
{
    while (!game_over)
    {
        uBit.sleep(BULLET_SPEED);

        if (bullet.y > -1){
          bullet.y--;
        }

        if (bullet.y == 0){
          sendBits(bullet.x);
          bullet.x = -1;
        }

        //If bullet collides with shield, reset bullet.
        if (shootout.getPixelValue(bullet.x, bullet.y) > 0)
        {
            shootout.setPixelValue(bullet.x, bullet.y, 0);
            bullet.x = -1;
            bullet.y = -1;
        }
    }
}

//Checks whether there is a shield tile at a certain LED, returns 1 if there is
/*int checkShield (int x, int y)
{
    for(int i = 0; i < 4; i++)
    {
        if (shootout.getPixelValue(x, y) == 255)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}
*/

//Adds 3 shield tiles centered above the player, these can block bullets
/*void shield(MicroBitEvent)
{
    for(int x = player.x - 1; x < player.x + 2; x++)
    {
        shootout.setPixelValue(x, 3, 255);
    }
}*/

void listen(){
  while (!game_over){
    uBit.sleep(1);
    switch ((int)getBits()){
      case 0: incomingFire(0);
      break;
      case 1: incomingFire(1);
      break;
      case 2: incomingFire(2);
      break;
      case 3: incomingFire(3);
      break;
      case 4: incomingFire(4);
      break;
    }
  }
}

//Main game function, sets original attributes and triggers most functions
void shootoutGame()
{
    game_over = 0;
    player.x = 2;
    player.y = 4;
    bullet.x = -1;
    bullet.y = -1;
    incomingBullet.x = -1;
    incomingBullet.y = -1;

    //Event handlers for button presses, triggers bullet and shield
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, fire);
    //uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, shield);

    //Individual fibers for moving elements of the game, prevents freezing and general bugginess
    create_fiber(listen);
    create_fiber(incomingBulletUpdate);
    create_fiber(playerUpdate);
    create_fiber(bulletUpdate);

    //refreshes the screen preventing duplicates of screen elements
    while (!game_over)
    {
        uBit.sleep(10);
        uBit.display.image.paste(shootout);
        uBit.display.image.setPixelValue(player.x, player.y, 255);
        uBit.display.image.setPixelValue(bullet.x, bullet.y, 255);
    }

    //if(game_over == 1)
        //gameOver();

}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    //uBit.display.setDisplayMode(DISPLAY_MODE_BLACK_AND_WHITE);

    //Title message
    uBit.display.scroll("SHOOTOUT!");

    P0.setDigitalValue(0);
    P1.setDigitalValue(0);

    while (1) {
        shootoutGame();
    }
}
