/*
 * Message.c
 *
 *  Created on: 2018年11月8日
 *      Author: Shouyuan
 */
#include <ti/sysbios/knl/Queue.h>

#include <stdlib.h>
#include <string.h>
#include "Message.h"

void messageAddItem(Buffer * msg, char *name, int type, char *value)
{
    char name_len = strlen(name);                  //在name命名时需要在结尾加上'\0'，或使用“”号
    char type_name;
    char value_len;
    if (type == 0 | type == 1)
    {
        value_len = 4;
    }
    else if (type == 2)
    {
        value_len = strlen(value);     //在char数据时需要在结尾加上'\0',或使用“”号
    }
    else if (type == 3)
    {
        value_len = 6;
    }
    int i;

    if (msg->pi < (msg->length - name_len - value_len))     //未超出申请的长度
    {
        type_name = ((char) type << 5) + (name_len & 0x1F);  //生成type+name_len
        msg->p[msg->pi++] = type_name;                //在msg加入type+name_len
        for (i = 0; i < name_len; i++)
        {
            msg->p[msg->pi++] = name[i];              //在msg加入name
        }
        switch (type)
        {
        case 0:              //数据类型为float
        {
            for (i = 1; i >= 0; i--)
            {
                msg->p[msg->pi++] = (value[i] >> 8) & 0xFF;
                msg->p[msg->pi++] = value[i] & 0xFF;              //在msg加入value
            }
            break;
        }
        case 1:                                       //数据类型为int
        {
            msg->p[msg->pi++] = 0;
            msg->p[msg->pi++] = 0;
            msg->p[msg->pi++] = (*(value) >> 8) & 0xFF;
            msg->p[msg->pi++] = *(value) & 0xFF;              //在msg加入value
            break;
        }
        case 2:                                         //数据类型为char
        {
            msg->p[msg->pi++] = value_len;                //在msg加入value_len
            for (i = 0; i < value_len; i++)
            {
                msg->p[msg->pi++] = value[i];              //在msg加入value
            }
            break;
        }
        case 3:                                        //数据类型为时标
        {
            for (i = 0; i < 6; i++)
            {
                msg->p[msg->pi++] = value[i];              //在msg加入value
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {       //超出了申请的长度

    }

}

void addXOR(Buffer *msg)       //给消息加异或校验位
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
    Buffer *msg = (Buffer *) malloc(sizeof(Buffer));    //定义与初始化（允许同时新建多个msg）
    msg->pi = 0;                                       //结构体指针
    msg->length = 0;
    msg->p = (char*) malloc(sizeof(char) * space); //分配n个存储单元，并将这n个连续的存储单元的首地址存储到指针变量p中
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

Message * parseData(char *rxbuf ) //参数（rxBuf数组，rxBuf数组长度，dataframe存取数据的结构体数组）
{
    Message * message;
    char nameLen;
    char valueLen;
    char rxXOR = 0;
    int parseIdx = 0;
    char m;
    int length;
    length = (rxbuf[3] << 8) + rxbuf[4]+6; //数据长度
    for (parseIdx = MSG_START; parseIdx < length - 1; parseIdx++) //-1除去校验位进行异或计算
    {
        rxXOR ^= rxbuf[parseIdx];
    }

    if (rxXOR == rxbuf[length - 1])  //异或校验
    {
        message=(Message *)malloc(sizeof(Message));
        message->dataQueue = Queue_create(NULL, NULL);
        DataFrame * dataframe;
        message->messageType = (MSGTYPE) ((rxbuf[2] >> 5) & 0x07);
        parseIdx = DATA_START;
        while (parseIdx < length - 1)
        {
            dataframe = (DataFrame *) malloc(sizeof(DataFrame));
            dataframe->type = ((rxbuf[parseIdx] >> 5)) & 0x07;  //数据类型
            nameLen = rxbuf[parseIdx++] & 0x1F;  //数据名称长度
            dataframe->name = (char *) malloc(sizeof(char) * nameLen);

            for (m = 0; m < nameLen; m++)  //存数据名称
            {
                dataframe->name[m] = rxbuf[parseIdx++];
            }

            switch (dataframe->type)
            //判断存取的数据类型
            {
            case 0:  //float类型解析
            {
                long temp = 0;
                long *p = &temp;
                for (m = 3; m >= 0; m--)  //高位在前，左移8*m位
                {
                    temp += (long) rxbuf[parseIdx++] << (8 * m);
                }
                dataframe->value.floating = *((float*) p);
                break;
            }
            case 1:  //int类型解析
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
            case 2:  //char类型解析
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

