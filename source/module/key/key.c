/*
 * key.c
 *
 *  Created on: 2018Äê11ÔÂ15ÈÕ
 *      Author: Shouyuan
 */

#include <ti/sysbios/hal/Hwi.h>
#include "DeviceHeaders/include/DSP2803x_Device.h"
#include "key.h"

KeyCallbackFunction callbackFun;

Hwi_Handle keyHwiHandle;

void keyEnable()
{
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;
    Hwi_enableInterrupt(KEY_INT_NUM);
}

void keyIntISR(UArg arg)
{
    callbackFun();
}

int keyInit(KeyCallbackFunction callback)
{
    if (callback == NULL)
    {
        return 1;
    }
    Hwi_Params hwiParam;
    Hwi_Params_init(&hwiParam);
    hwiParam.enableInt = 0;
    keyHwiHandle = Hwi_create(KEY_INT_NUM, keyIntISR, &hwiParam, NULL);
    if (keyHwiHandle == NULL)
    {
        return 1;
    }
    callbackFun = callback;

    XIntruptRegs.XINT1CR.bit.ENABLE = 0;
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;
    GpioCtrlRegs.GPACTRL.bit.QUALPRD1 = 0XFF;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 2;
    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 12;
    EDIS;

    return 0;
}
