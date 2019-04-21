/*
 * Message.h
 *
 *  Created on: 2018��11��8��
 *      Author: Shouyuan
 */

#ifndef SOURCE_TRANSFER_MESSAGE_H_
#define SOURCE_TRANSFER_MESSAGE_H_

#include <ti/sysbios/knl/Queue.h>
#include "Common.h"

#define TYPE_CHAR 2
#define MSG_START 2//��ȥ�̶���'#','S'�����ӵ�3�ֽڿ�ʼ����
#define DATA_START 5//��ȥ����ͷ���������ݴӵ�6�ֽڿ�ʼ

typedef enum
{
    CONNECT,
    CONNACK,
    DOWNSIDE,
    UPSIDE,
    DOWNACK,
    KEEPALIVE
}MSGTYPE;

typedef struct {    //����ṹ��Msg
    char *p;           //��ʼָ��
    int pi;            //�������ݳ���
    int length;        //������ݳ���
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
