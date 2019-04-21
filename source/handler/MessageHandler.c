/*
 * MessageHandler.c
 *
 *  Created on: 2018Äê11ÔÂ8ÈÕ
 *      Author: Shouyuan
 */
#include <string.h>

#include "../core/Core.h"
#include "../module/transfer/Message.h"
#include "../module/transfer/TransferAction.h"
#include "../module/transfer/Transfer.h"
#include "../module/led/led.h"
#include "MessageHandler.h"

void MessageHandler_msg(CoreArg arg)
{
    char *msg = (char *) arg;
    Message * parseMessage = parseData(msg);
    if (parseMessage == NULL)
    {
        return;
    }

    switch (parseMessage->messageType)
    {
    case CONNACK:
    {
        //corePutWork(CORE_WORK_PRIORITY_LOW,fuck,NULL,NULL);
        break;
    }
    case KEEPALIVE:
    {
        corePutWork(CORE_WORK_PRIORITY_LOW, KeepAlive, 1, NULL);
    }
    case DOWNSIDE:
    {
        DataFrame *ptr;
        ptr = Queue_head(parseMessage->dataQueue);
        if (strncmp(ptr->name, "DA", strlen("DA")) == 0)
        {
            corePutWork(CORE_WORK_PRIORITY_LOW, Upside, 1, NULL);
        }
        else if (strncmp(ptr->name, "SW", strlen("SW")) == 0)
        {
            ledSet(*(ptr->value.string) == '1' ? LED_ST_ON : LED_ST_OFF);
            corePutWork(CORE_WORK_PRIORITY_LOW, Downack, 1, NULL);
        }

    }
    }
    free_parse(parseMessage);
}

int messageHandlerInit()
{
    return Transfer_init(MessageHandler_msg);

}
