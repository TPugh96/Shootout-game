#include "MicroBit.h"

MicroBit uBit;
MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_ALL);
MicroBitPin P1(MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_ALL);

int binary [3] = {0,0,0};



void sendMessage(){
  P0.setDigitalValue(1);
  uBit.display.print("C");
  uBit.sleep(300);
  //uBit.sleep(20);
  P0.setDigitalValue(0);
}

void sendBinary(MicroBitEvent){
  P0.setDigitalValue(1);
  wait_us(100000);
  P0.setDigitalValue(0);
  wait_us(100000);
  P0.setDigitalValue(0);
  wait_us(100000);
  P0.setDigitalValue(1);
}

void receiveBinary(){
  if (P1.getDigitalValue() == 1){
    wait_us(100000);
    binary[0] = P1.getDigitalValue();
    wait_us(100000);
    binary[1] = P1.getDigitalValue();
    wait_us(100000);
    binary[2] = P1.getDigitalValue();
    uBit.sleep(100);
  }
}

void commsTest(){

  //sendBinary();
  //P0.setDigitalValue(0);
  //create_fiber(receiveBinary);




  //int binary [3] = {0,0,0};

  uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, sendBinary);
  //uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, sendMessage);

  //while (true){



    /*if (uBit.buttonA.isPressed() == 1){
      sendBinary();
    }*/
    /*if (P1.getDigitalValue() == 1){
      wait_us(100000);
      binary[0] = P1.getDigitalValue();
      wait_us(100000);
      binary[1] = P1.getDigitalValue();
      wait_us(100000);
      binary[2] = P1.getDigitalValue();
      wait_us(100000);


      /*for (int k = 0 ; k < 3 ; k++){
        uBit.display.print(binary[k]);
        wait_us(100000);
        uBit.display.clear();
      }*/
      //uBit.display.print("Y");
      //uBit.sleep(300);
    //} else {
      //uBit.display.print("X");
      //uBit.sleep(300);
    //}
  //}
}


void send(){
  P0.setDigitalValue(1);
  uBit.display.print("X");
}

void receive(){
  if (P1.getDigitalValue() == 1){
    uBit.display.print("Y");
  }
}



int main(){
  uBit.init();
  P0.setDigitalValue(0);
  P1.setDigitalValue(0);

  while (true){
    commsTest();
    //send();
    //receive();
  }


}
