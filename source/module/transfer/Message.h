/*
 * Message.h
 *
 *  Created on: 2018年11月8日
 *      Author: Shouyuan
 */

#ifndef SOURCE_TRANSFER_MESSAGE_H_
#define SOURCE_TRANSFER_MESSAGE_H_

#include <ti/sysbios/knl/Queue.h>
#include "Common.h"

#define TYPE_CHAR 2
#define MSG_START 2//除去固定字'#','S'，异或从第3字节开始计算
#define DATA_START 5//除去数据头，有用数据从第6字节开始

typedef enum
{
    CONNECT,
    CONNACK,
    DOWNSIDE,
    UPSIDE,
    DOWNACK,
    KEEPALIVE
}MSGTYPE;

typedef struct {    //定义结构体Msg
    char *p;           //初始指针
    int pi;            //现有数据长度
    int length;        //最大数据长度
} Buffer;


typedef struct{
    Queue_Elem elem;
    char type;
    char *name;
    union{
        float floating;
        int integer;
        char *string;
    } value;
} DataFrame;
typedef struct{
    MSGTYPE messageType;
    Queue_Handle dataQueue;
}Message;



void messageAddItem(Buffer * msg,char *name ,int type ,char *value);
Buffer * messageCreate(int space);
void messageFree(Buffer * msg);
void addXOR(Buffer *msg);

Message * parseData(char *rxbuf);

void free_parse(Message *message);


#endif /* SOURCE_TRANSFER_MESSAGE_H_ */
