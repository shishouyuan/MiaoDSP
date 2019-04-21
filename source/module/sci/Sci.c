/*
 * Sci.c
 *
 *  Created on: 2018年11月8日
 *      Author: Shouyuan
 */

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <stdlib.h>

#include <DeviceHeaders/include/DSP2803x_Device.h>
#include "Sci.h"

Semaphore_Struct sciSendSemStruct;
Semaphore_Handle sciSendSemHd;

Hwi_Struct sciSendHwiStruct;
Hwi_Handle sciSendHwiHd;

Hwi_Struct sciTxHwiStruct;
Hwi_Handle sciTxHwiHd;

Hwi_Struct sciRxHwiStruct;
Hwi_Handle sciRxHwiHd;

SciRxCallback rxCallback;

int txIdx = 0; //发送数据索引
int txLen = 0; //发送数据的长度
char *txBuf = NULL;

void enableRxInt()
{
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;
    Hwi_enableInterrupt(SCI_RX_INT_NUM);
}

void disableRxInt()
{
    SciaRegs.SCICTL2.bit.RXBKINTENA = 0;
    Hwi_disableInterrupt(SCI_RX_INT_NUM);
}

void enableTxInt()
{
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    Hwi_enableInterrupt(SCI_TX_INT_NUM);
}

void disableTxInt()
{
    SciaRegs.SCICTL2.bit.TXINTENA = 0;
    Hwi_disableInterrupt(SCI_TX_INT_NUM);
}

int sciIsBusy()
{
    return !Semaphore_getCount(sciSendSemHd);
}

int sciTrySend(char *data, UInt16 length, UInt32 waittime)
{
    int i;
    if (!Semaphore_pend(sciSendSemHd, waittime))
    {
        return 1;
    }
    txLen = length;
    if(txBuf!=NULL)
    {
        free(txBuf);
    }
    txBuf = (char *) malloc(sizeof(char) * txLen);
    if (txBuf == NULL)
    {
        Semaphore_post(sciSendSemHd);
        return 1;
    }

    for (i = 0; i < txLen; i++)
    {
        //*(txBuf + count) = *(data + count);
        txBuf[i] = data[i];
    }

    //SciaRegs.SCITXBUF = *(txBuf + txIdx);

    enableTxInt();
    txIdx = 1;
    SciaRegs.SCITXBUF = txBuf[0];

    return 0;

}

int sciSend(char *data, UInt16 length)
{
    return sciTrySend(data, length, BIOS_WAIT_FOREVER);
}

void sci_tx_isr(UArg arg)
{
    if (txBuf==NULL || txIdx >= txLen)
    {
        disableTxInt();

        txIdx = 0;

        if(txBuf!=NULL){
            free((void*) txBuf);
            txBuf = NULL;
        }

        Semaphore_post(sciSendSemHd);
    }
    else
    {
        SciaRegs.SCITXBUF = txBuf[txIdx];
        txIdx++;
    }
}

void sci_rx_isr(UArg arg)
{
    rxCallback(SciaRegs.SCIRXBUF.bit.RXDT);
}

void sciInitRegs()
{

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;

    /* Enable internal pull-up for the selected pins */
    // Pull-ups can be enabled or disabled disabled by the user.
    // This will enable the pullups for the specified pins.
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;   // Enable pull-up for GPIO28 (SCIRXDA)
    //  GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;     // Enable pull-up for GPIO7  (SCIRXDA)

    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;   // Enable pull-up for GPIO29 (SCITXDA)
    //  GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;    // Enable pull-up for GPIO12 (SCITXDA)

    /* Set qualification for selected pins to asynch only */
    // Inputs are synchronized to SYSCLKOUT by default.
    // This will select asynch (no qualification) for the selected pins.
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  // Asynch input GPIO28 (SCIRXDA)
    //  GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 3;   // Asynch input GPIO7 (SCIRXDA)

    /* Configure SCI-A pins using GPIO regs*/
    // This specifies which of the possible GPIO pins will be SCI functional pins.
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1; // Configure GPIO28 for SCIRXDA operation
    //  GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 2;    // Configure GPIO7  for SCIRXDA operation

    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1; // Configure GPIO29 for SCITXDA operation
    //  GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 2;   // Configure GPIO12 for SCITXDA operation

    EDIS;

    /*SCI寄存器配置*/
    SciaRegs.SCICTL1.bit.SWRESET = 0;
    SciaRegs.SCICCR.all = 0x0007;  //1位停止位，8位数据位，无奇偶校验位
    SciaRegs.SCIHBAUD = 0x00;  //BRR=(LSPCLK/Baud*8)-1     LSPCLK=15MHz
    SciaRegs.SCILBAUD = 0x30;  //BRR=48,BAUD=38400
    //  SciaRegs.SCILBAUD=0xC2;  //BRR=194,BAUD=9600
    //  SciaRegs.SCILBAUD=0x61;  //BRR=97,BAUD=19200;
    //  SciaRegs.SCILBAUD=0x0F;  //BRR=15,BAUD=115200;
    //  SciaRegs.SCILBAUD=0x0E;  //BRR=14,BAUD=128000;
    SciaRegs.SCICTL1.all = 0x0023;    //使能SCI发送、SCI接收
}



int sciInit(SciRxCallback cb)
{
    if (cb == NULL)
    {
        return 1;
    }


    rxCallback = cb;

    sciInitRegs();

    Semaphore_Params semParam;
    Semaphore_Params_init(&semParam);
    semParam.mode = Semaphore_Mode_BINARY;
    Semaphore_construct(&sciSendSemStruct, 1, &semParam);
    sciSendSemHd = Semaphore_handle(&sciSendSemStruct);
    if (sciSendSemHd == NULL)
    {
        return 1;
    }

    Hwi_Params hwiParam;

    Hwi_Params_init(&hwiParam);
    hwiParam.enableInt = 0;
    Hwi_construct(&sciRxHwiStruct, SCI_RX_INT_NUM, sci_rx_isr, &hwiParam, NULL);
    sciRxHwiHd = Hwi_handle(&sciRxHwiStruct);
    if (sciRxHwiHd == NULL)
    {
        return 1;
    }

    Hwi_Params_init(&hwiParam);
    hwiParam.enableInt = 0;
    Hwi_construct(&sciTxHwiStruct, SCI_TX_INT_NUM, sci_tx_isr, &hwiParam, NULL);
    sciTxHwiHd = Hwi_handle(&sciTxHwiStruct);
    if (sciTxHwiHd == NULL)
    {
        return 1;
    }


    enableRxInt();
    return 0;
}


