/*
 * Message.c
 *
 *  Created on: 2018��11��8��
 *      Author: Shouyuan
 */
#include <ti/sysbios/knl/Queue.h>

#include <stdlib.h>
#include <string.h>
#include "Message.h"

void messageAddItem(Buffer * msg, char *name, int type, char *value)
{
    char name_len = strlen(name);                  //��name����ʱ��Ҫ�ڽ�β����'\0'����ʹ�á�����
    char type_name;
    char value_len;
    if (type == 0 | type == 1)
    {
        value_len = 4;
    }
    else if (type == 2)
    {
        value_len = strlen(value);     //��char����ʱ��Ҫ�ڽ�β����'\0',��ʹ�á�����
    }
    else if (type == 3)
    {
        value_len = 6;
    }
    int i;

    if (msg->pi < (msg->length - name_len - value_len))     //δ��������ĳ���
    {
        type_name = ((char) type << 5) + (name_len & 0x1F);  //����type+name_len
        msg->p[msg->pi++] = type_name;                //��msg����type+name_len
        for (i = 0; i < name_len; i++)
        {
            msg->p[msg->pi++] = name[i];              //��msg����name
        }
        switch (type)
        {
        case 0:              //��������Ϊfloat
        {
            for (i = 1; i >= 0; i--)
            {
                msg->p[msg->pi++] = (value[i] >> 8) & 0xFF;
                msg->p[msg->pi++] = value[i] & 0xFF;              //��msg����value
            }
            break;
        }
        case 1:                                       //��������Ϊint
        {
            msg->p[msg->pi++] = 0;
            msg->p[msg->pi++] = 0;
            msg->p[msg->pi++] = (*(value) >> 8) & 0xFF;
            msg->p[msg->pi++] = *(value) & 0xFF;              //��msg����value
            break;
        }
        case 2:                                         //��������Ϊchar
        {
            msg->p[msg->pi++] = value_len;                //��msg����value_len
            for (i = 0; i < value_len; i++)
            {
                msg->p[msg->pi++] = value[i];              //��msg����value
            }
            break;
        }
        case 3:                                        //��������Ϊʱ��
        {
            for (i = 0; i < 6; i++)
            {
                msg->p[msg->pi++] = value[i];              //��msg����value
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {       //����������ĳ���

    }

}

void addXOR(Buffer *msg)       //����Ϣ�����У��λ
{
    int i;
    char txXOR = 0;
    msg->p[3] = (msg->pi >> 8) & 0xFF;
    msg->p[4] = (msg->pi - 5) & 0xFF;
    for (i = 2; i < msg->pi; i++)
    {
        txXOR ^= msg->p[i];
    }
    msg->p[msg->pi++] = txXOR;
}

Buffer *messageCreate(int space)
{
    Buffer *msg = (Buffer *) malloc(sizeof(Buffer));    //�������ʼ��������ͬʱ�½����msg��
    msg->pi = 0;                                       //�ṹ��ָ��
    msg->length = 0;
    msg->p = (char*) malloc(sizeof(char) * space); //����n���洢��Ԫ��������n�������Ĵ洢��Ԫ���׵�ַ�洢��ָ�����p��
    msg->p[msg->pi++] = MESSAGE_H1;
    msg->p[msg->pi++] = MESSAGE_H2;
    msg->p[msg->pi++] = PROTOCOL_VERSION;
    msg->pi = msg->pi + 2;
    msg->length = space;
    return msg;
}

void messageFree(Buffer * msg)
{
    free(msg->p);
    free(msg);
}

Message * parseData(char *rxbuf ) //������rxBuf���飬rxBuf���鳤�ȣ�dataframe��ȡ���ݵĽṹ�����飩
{
    Message * message;
    char nameLen;
    char valueLen;
    char rxXOR = 0;
    int parseIdx = 0;
    char m;
    int length;
    length = (rxbuf[3] << 8) + rxbuf[4]+6; //���ݳ���
    for (parseIdx = MSG_START; parseIdx < length - 1; parseIdx++) //-1��ȥУ��λ����������
    {
        rxXOR ^= rxbuf[parseIdx];
    }

    if (rxXOR == rxbuf[length - 1])  //���У��
    {
        message=(Message *)malloc(sizeof(Message));
        message->dataQueue = Queue_create(NULL, NULL);
        DataFrame * dataframe;
        message->messageType = (MSGTYPE) ((rxbuf[2] >> 5) & 0x07);
        parseIdx = DATA_START;
        while (parseIdx < length - 1)
        {
            dataframe = (DataFrame *) malloc(sizeof(DataFrame));
            dataframe->type = ((rxbuf[parseIdx] >> 5)) & 0x07;  //��������
            nameLen = rxbuf[parseIdx++] & 0x1F;  //�������Ƴ���
            dataframe->name = (char *) malloc(sizeof(char) * nameLen);

            for (m = 0; m < nameLen; m++)  //����������
            {
                dataframe->name[m] = rxbuf[parseIdx++];
            }

            switch (dataframe->type)
            //�жϴ�ȡ����������
            {
            case 0:  //float���ͽ���
            {
                long temp = 0;
                long *p = &temp;
                for (m = 3; m >= 0; m--)  //��λ��ǰ������8*mλ
                {
                    temp += (long) rxbuf[parseIdx++] << (8 * m);
                }
                dataframe->value.floating = *((float*) p);
                break;
            }
            case 1:  //int���ͽ���
            {
                long temp = 0;
                long *p = &temp;
                for (m = 3; m >= 0; m--)
                {
                    temp += (long) rxbuf[parseIdx++] << (8 * m);
                }
                dataframe->value.integer = *((int*) p);
                break;
            }
            case 2:  //char���ͽ���
            {
                valueLen = rxbuf[parseIdx++];
                dataframe->value.string = (char *) malloc(
                        sizeof(char) * valueLen);
                for (m = 0; m < valueLen; m++)
                {
                    dataframe->value.string[m] = rxbuf[parseIdx++];
                }
                break;
            }
            default:
                break;
            }
            Queue_enqueue(message->dataQueue, &(dataframe->elem));
        }
        return message;
    }
    else
    {
        return NULL;
    }

}

void free_parse(Message * message)
{
    DataFrame *p;
    while (!Queue_empty(message->dataQueue))
    {
        p = Queue_dequeue(message->dataQueue);
        free(p->name);
        p->name = NULL;
        if (p->type == TYPE_CHAR)
        {
            free(p->value.string);
            p->value.string = NULL;
        }
        free(p);
        p = NULL;
    }
    Queue_delete(&message->dataQueue);
    free(message);
}

