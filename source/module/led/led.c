/*
 * led.c
 *
 *  Created on: 2018Äê11ÔÂ9ÈÕ
 *      Author: Shouyuan
 */
#include <DeviceHeaders/include/DSP2803x_Device.h>
#include "led.h"

#define LED_Port GpioDataRegs.GPBDAT.bit.GPIO34

LEDState ledState()
{
    return (LEDState)LED_Port;
}

void ledSet(LEDState st){
    LED_Port=st;
}

int ledInit(){
    EALLOW;
        GpioCtrlRegs.GPBMUX1.bit.GPIO34=0;
        GpioCtrlRegs.GPBDIR.bit.GPIO34=1;
    EDIS;

    ledSet(LED_ST_ON);
    return 0;
}
