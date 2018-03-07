#include "MicroBit.h"
#include "multiplayer.cpp"

MicroBit uBit;
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_ALL);
MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_ALL);
int binary[3];
int x_value;

int decode(int received[]){
  if ((received[0] == 0) & (received[1] == 0) & (received[2] == 1)){
    return 1;
  } else if ((received[0] == 0) & (received[1] == 1) & (received[2] == 0)){
    return 2;
  } else if ((received[0] == 0) & (received[1] == 1) & (received[2] == 1)){
    return 3;
  } else if ((received[0] == 1) & (received[1] == 0) & (received[2] == 0)){
    return 4;
  } else if ((received[0] == 1) & (received[1] == 0) & (received[2] == 1)){
    return 5;
  } else {
    return 0;
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

void send(bulletX){
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

// void runProgram(){

//   create_fiber(listen);
//   create_fiber(send);
//   //uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, send, MESSAGE_BUS_LISTENER_IMMEDIATE);

//   while (true){
//     uBit.sleep(1);
//     for (int k = 0 ; k < 3 ; k++){
//       uBit.display.scroll(binary[k]);
//     }
//     uBit.display.scroll("X");
//   }
// }

