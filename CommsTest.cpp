#include "MicroBit.h"

MicroBit uBit;
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_ALL);
MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_ALL);
int binary [3] = {0,0,0};

void listen(){
  while (true){
    if (P1.getDigitalValue() == 1){
      wait_us(500);
      binary[0] = P1.getDigitalValue();
      wait_us(500);
      binary[1] = P1.getDigitalValue();
      wait_us(500);
      binary[2] = P1.getDigitalValue();
      wait_us(500);
    }
    uBit.sleep(1);
  }
}

void send(MicroBitEvent){
  //Will be turned into a multi use function with paramenters. Currently used for testing.

  P0.setDigitalValue(1);
  wait_us(500);
  P0.setDigitalValue(0);
  wait_us(500);
  P0.setDigitalValue(0);
  wait_us(500);
  P0.setDigitalValue(1);
  wait_us(500);

  //Reset
  //P0.setDigitalValue(0);
}

void runProgram(){

  create_fiber(listen);
  uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, send, MESSAGE_BUS_LISTENER_IMMEDIATE);

  while (true){
    uBit.sleep(1);
    uBit.display.print(binary[2]);
    //uBit.display.print(P1.getDigitalValue());
  }
}


int main (){
  uBit.init();
  P0.setDigitalValue(0);
  P1.setDigitalValue(0);

  while(true){
    runProgram();
  }
}
