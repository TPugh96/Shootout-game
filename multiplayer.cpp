//Author: Thomas Pugh (16021460)

#include "MicroBit.h"

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


void send(int bulletX){
  //Will be turned into a multi use function with paramenters. Currently used for testing.
  while (true){
    switch(bulletX){
      case 0:
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(0);
        wait_ms(25);
        P0.setDigitalValue(0);
        wait_ms(25);
        P0.setDigitalValue(1);
        P0.setDigitalValue(0); // These last ones were put in to try and clear the inputs.
      break;
      case 1:
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(0);
        wait_ms(25);
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(0);
        P0.setDigitalValue(0);
      break;
      case 2:
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(0);
        wait_ms(25);
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(1);
        P0.setDigitalValue(0);
      break;
      case 3:
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(0);
        wait_ms(25);
        P0.setDigitalValue(0);
        P0.setDigitalValue(0);
      break;
      case 4:
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(1);
        wait_ms(25);
        P0.setDigitalValue(0);
        wait_ms(25);
        P0.setDigitalValue(1);
        P0.setDigitalValue(0);
    }
    uBit.sleep(1);
  }
}

int decode(int received[]){
  if ((received[0] == 0) & (received[1] == 0) & (received[2] == 1)){
    return 0;
  } else if ((received[0] == 0) & (received[1] == 1) & (received[2] == 0)){
    return 1;
  } else if ((received[0] == 0) & (received[1] == 1) & (received[2] == 1)){
    return 2;
  } else if ((received[0] == 1) & (received[1] == 0) & (received[2] == 0)){
    return 3;
  } else if ((received[0] == 1) & (received[1] == 0) & (received[2] == 1)){
    return 4;
  } else {
    return 5;
  }
}

void listen(){
  while (true){
    if (P1.getDigitalValue() == 1){
      wait_ms(25);
      binary[0] = P1.getDigitalValue();
      wait_ms(25);
      binary[1] = P1.getDigitalValue();
      wait_ms(25);
      binary[2] = P1.getDigitalValue();
      wait_ms(25);
      x_value = decode(binary);
      incomingFire(x_value);
    }
    uBit.sleep(1);
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
        if (incomingBullet.y > 4)
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
    int bulletX;

    while (!game_over)
    {
        uBit.sleep(BULLET_SPEED);
        if (bullet.y != -1)
            bullet.y--;

        //If bullet goes out of bounds it is reset until fire is triggered again
        if (bullet.y == -1)
        {
            bulletX = bullet.x;
            send(bulletX);
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
//Bullet movement function for player 2
/*void bulletUpdate2()
{

    for(int i = 0; i < 5; i++)
    {
        uBit.sleep(100);
        if (bullet2.y != -1)
        {
            bullet2.y++;
        }
        if (bullet2.y > 4)
        {
            shootout.setPixelValue(bullet2.x, bullet2.y, 0);
            bullet2.x = -1;
            bullet2.y = -1;
        }

        //If there is a shield tile in the bullets path, both are removed
        if (checkShield(bullet2.x, bullet2.y) == 1)
        {
            shootout.setPixelValue(bullet2.x, 3, 0);
            bullet2.x = -1;
            bullet2.y = -1;
            shootout.setPixelValue(bullet2.x, 3, 0);
        }

        //Checks whether bullet has hit player, if so bullet is reset and point added to player 2
        if (bullet2.x == player.x && bullet2.y == player.y)
        {
            player2Score++;
            shootout.setPixelValue(bullet2.x, bullet2.y, 0);
            bullet2.x = -1;
            bullet2.y = -1;
        }

        //Game ends if player 2 score reaches 3
        if (player2Score == 3)
        {
            game_over = 1;
        }
    }
}
*/

//Adds 3 shield tiles centered above the player, these can block bullets
void shield(MicroBitEvent)
{
    for(int x = player.x - 1; x < player.x + 2; x++)
    {
        shootout.setPixelValue(x, 3, 255);
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
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, shield);

    //Individual fibers for moving elements of the game, prevents freezing and general bugginess
    create_fiber(playerUpdate);
    create_fiber(bulletUpdate);
    create_fiber(incomingBulletUpdate);
    create_fiber(listen);

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

    //Title message
    uBit.display.scroll("Shootout");

    P0.setDigitalValue(0);
    P1.setDigitalValue(0);

    while (1) {
        shootoutGame();
    }
}
