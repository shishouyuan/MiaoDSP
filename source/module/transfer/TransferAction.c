/*
 * TransferAction.c
 *
 *  Created on: 2018年11月8日
 *      Author: Shouyuan
 */
#include "../../core/Core.h"
#include "Message.h"
#include "TransferAction.h"
#include "../led/led.h"
#include "../sci/Sci.h"

extern char ID[];
extern char TYPE[];
char wait=0;//收到downside类型置零
float Vrms=230.0201;
float t=26.5;
int num=0;


void KeepAlive(CoreArg para)
{
    Buffer *buf;
    MSGTYPE txMsgType=KEEPALIVE;
    buf=messageCreate(10);
    buf->p[2]|=txMsgType<<5;
    addXOR(buf);
    sciSend(buf->p, buf->pi);
    messageFree(buf);
}

void Upside(CoreArg para)
{
    Buffer *buf;
    MSGTYPE txMsgType=UPSIDE;
    buf=messageCreate(512);
    buf->p[2]|=txMsgType<<5;
//  add_payload(buf,"ID",2,ID);
    messageAddItem(buf,"V",0,(char*)&Vrms);
    messageAddItem(buf,"t",0,(char*)&t);
    messageAddItem(buf,"n",1,(char*)&num);
    addXOR(buf);
    num++;
    sciSend(buf->p, buf->pi);
    messageFree(buf);
}

void Downack(CoreArg para)
{
    Buffer *buf;
    MSGTYPE txMsgType=DOWNACK;
    buf=messageCreate(20);
    buf->p[2]|=txMsgType<<5;
    //add_payload(buf,"SW",2,"1");//char类型
    if(ledState()==LED_ST_ON)
    {
        messageAddItem(buf,"SW",2,"1");
    }
    else
    {
        messageAddItem(buf,"SW",2,"0");
    }
    addXOR(buf);
    sciSend(buf->p, buf->pi);
    messageFree(buf);
}

