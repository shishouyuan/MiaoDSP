/*
 * KeyHandler.c
 *
 *  Created on: 2018Äê11ÔÂ15ÈÕ
 *      Author: Shouyuan
 */

#include "../module/key/key.h"
#include "../module/led/led.h"
#include "../module/transfer/TransferAction.h"
#include "KeyHandler.h"

void keyHandlerFun(){
    ledSet((LEDState)!ledState());
    Downack(0);
}

int KeyHandlerInit(){
    int r= keyInit(keyHandlerFun);
    if(!r)
        keyEnable();
    return r;
}


