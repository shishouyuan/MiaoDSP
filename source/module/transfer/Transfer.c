/*
 * TransferModule.c
 *
 *  Created on: 2018年11月7日
 *      Author: Shouyuan
 */
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <stdlib.h>

#include <DeviceHeaders/include/DSP2803x_Device.h>
#include "../sci/Sci.h"
#include "../../core/core.h"
#include "Transfer.h"

HandleFunction parseHandler;

Clock_Struct clockStruct;
Clock_Handle clockHd;

int rxLen = 0; //接收数组的长度
int rxEnd = 0; //数据接收完成标志
int rxIdx = 0; //接收数据索引
int rxDataLen = 0; //接收数据的长度
volatile char *rxBuf = NULL;
char rxHeader[5] = { 0, 0, 0, 0, 0 };
int headerIdx = 0;
char temp = 0;

char received = 0;

void TransferRxCallback(char data)
{
    int i;
    rxEnd = 0;
    switch (headerIdx)
    {
    case 0:
        rxHeader[headerIdx] = data;
        if (rxHeader[headerIdx] == MESSAGE_H1)
        {
            headerIdx++;
        }
        else
        {
            rxHeader[headerIdx] = 0;
        }
        break;
    case 1:
        rxHeader[headerIdx] = data;
        if (rxHeader[headerIdx] == MESSAGE_H2)
        {
            headerIdx++;
        }
        else
        {
            for (i = 0; i <= headerIdx; i++)
            {
                rxHeader[i] = 0;
            }
            headerIdx = 0;
        }
        break;
    case 2:
        rxHeader[headerIdx] = data;
        headerIdx++;
        break;
    case 3:
        rxHeader[headerIdx] = data;
        temp = rxHeader[headerIdx] << 8; //长度高8位
        headerIdx++;
        break;
    case 4:
        rxHeader[headerIdx] = data;
        rxDataLen = temp + rxHeader[headerIdx]; //长度高8位+低8位;
        headerIdx++;
        rxLen = rxDataLen + headerIdx + 1; //+1校验位为总接收长度
        temp = 0;
        rxBuf = (char *) malloc(sizeof(char) * rxLen);
        if (rxBuf == NULL)
        {
            //要解决
        }
        for (rxIdx = 0; rxIdx < headerIdx; rxIdx++)
        {
            //*(rxBuf + rxIdx) = rxHeader[rxIdx];
            rxBuf[rxIdx] = rxHeader[rxIdx];
            rxHeader[rxIdx] = 0;
        }
        break;
    default:
        //*(rxBuf + rxIdx) = SciaRegs.SCIRXBUF.bit.RXDT;
        rxBuf[rxIdx] = data;
        rxIdx++;
        if (rxIdx == rxLen)
        {
            //corePutWork(CORE_WORK_PRIORITY_LOW, ph, (CoreArg)rxBuf, free);
            corePutWork(CORE_WORK_PRIORITY_LOW, parseHandler, (CoreArg) rxBuf,
                        free);
            rxBuf = NULL;
            rxIdx = 0;
            headerIdx = 0;
            received = 1;
            Clock_start(clockHd);
        }
    }
}



void Connect(CoreArg arg)
{
    Buffer *request;
    MSGTYPE txMsgType=CONNECT;

    request=messageCreate(30);

    request->p[2]|=txMsgType<<5;
    messageAddItem(request,"ID",2,"LAB312");
    messageAddItem(request,"TYP",2,"P");
    addXOR(request);
    sciSend(request->p, request->pi);

    messageFree(request);

}


void clockFun(UArg arg)
{
    if (received)
    {
        received = 0;
    }
    else
    {
        corePutWork(CORE_WORK_PRIORITY_LOW, Connect, NULL, NULL);
    }
}

int initClock()
{
    Clock_Params clockParam;
    Clock_Params_init(&clockParam);
    clockParam.period = TRANSFER_KEEP_ALIVE_TIMEOUT;
    clockParam.startFlag = FALSE;

    Clock_construct(&clockStruct, clockFun, TRANSFER_KEEP_ALIVE_TIMEOUT, &clockParam);
    clockHd = Clock_handle(&clockStruct);
    if (clockHd == NULL)
    {
        return 1;
    }
    return 0;
}

void Transfer_start(CoreArg arg){
    Connect(NULL);
    Clock_start(clockHd);
}

int Transfer_init(HandleFunction ph)
{
    if (ph == NULL || sciInit(TransferRxCallback))
    {
        return 1;
    }
    initClock();
    parseHandler = ph;
    return 0;
}

