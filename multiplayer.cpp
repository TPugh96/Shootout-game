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
bool game_over;
int hitsTaken;
bool winner;

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
        if (incomingBullet.y < 5){
          incomingBullet.y++;
        }

        //Checking if the incoming bullet hits the players location. Icrements
        //the hits taken.
        if ((incomingBullet.x == player.x) & (incomingBullet.y == player.y)){
          hitsTaken++;
        }

        //If bullet goes out of bounds it is reset until fire is triggered again
        if (shootout.getPixelValue(incomingBullet.x, incomingBullet.y) > 0)
        {
            shootout.setPixelValue(incomingBullet.x, incomingBullet.y, 0);
            incomingBullet.x = 5;
            incomingBullet.y = 5;
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
          //We're sending the x coord + 1 to negate the issue of 0.
          sendBits(bullet.x + 1);
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

//Adds 3 shield tiles centered above the player, these can block bullets
void shield(MicroBitEvent)
{
    for(int x = player.x - 1; x < player.x + 2; x++)
    {
        shootout.setPixelValue(x, 3, 255);
    }
}

void listen(){
  while (!game_over){
    uBit.sleep(1);
    //The values are inverted as to replicate the game as if the microbits are
    //being held facing eachother.
    switch ((int)getBits()){
      case 1: incomingBullet.y = 0;
              incomingBullet.x = 4;
      break;
      case 2: incomingBullet.y = 0;
              incomingBullet.x = 3;
      break;
      case 3: incomingBullet.y = 0;
              incomingBullet.x = 2;
      break;
      case 4: incomingBullet.y = 0;
              incomingBullet.x = 1;
      break;
      case 5: incomingBullet.y = 0;
              incomingBullet.x = 0;
      break;
      case 6: winner = true;
              game_over = true;
      break;
    }
  }
}

void gameOver(){
  if (winner){
    for (int k = 0 ; k < 2 ; k++){
      uBit.display.scroll("YOU WIN!");
    }
  } else {
    for (int k = 0 ; k < 2 ; k++){
      uBit.display.scroll("YOU LOSE!");
    }
  }
}

//Game over function.
void checkGameOver()
{
  while (!game_over){
    uBit.sleep(1);
    if (hitsTaken == 3){
      sendBits(6);
      game_over = true;
    }
  }
}

//Main game function, sets original attributes and triggers most functions
void shootoutGame()
{
    game_over = false;
    winner = false;
    hitsTaken = 0;
    player.x = 2;
    player.y = 4;
    bullet.x = -1;
    bullet.y = -1;
    incomingBullet.x = 5;
    incomingBullet.y = 5;

    //Event handlers for button presses, triggers bullet and shield
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, fire);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, shield);

    //Individual fibers for moving elements of the game, prevents freezing and general bugginess
    create_fiber(listen);
    create_fiber(incomingBulletUpdate);
    create_fiber(playerUpdate);
    create_fiber(bulletUpdate);
    create_fiber(checkGameOver);

    //refreshes the screen preventing duplicates of screen elements
    while (!game_over)
    {
        uBit.sleep(10);
        uBit.display.image.paste(shootout);
        uBit.display.image.setPixelValue(player.x, player.y, 255);
        uBit.display.image.setPixelValue(bullet.x, bullet.y, 255);
        uBit.display.image.setPixelValue(incomingBullet.x, incomingBullet.y, 255);
    }
    gameOver();
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    //Title message
    uBit.display.scroll("SHOOTOUT!");

    P0.setDigitalValue(0);
    P1.setDigitalValue(0);

    while (1) {
        shootoutGame();
    }
}
