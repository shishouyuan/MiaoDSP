/*
 * MessageHandler.h
 *
 *  Created on: 2018Äê11ÔÂ8ÈÕ
 *      Author: Shouyuan
 */

#ifndef SOURCE_HANDLER_MESSAGEHANDLER_H_
#define SOURCE_HANDLER_MESSAGEHANDLER_H_

#include "../core/Core.h"
#include "..\module\transfer\Message.h"

int messageHandlerInit();
void MessageHandler_msg(CoreArg arg);
int MessageHandler_parse(Message data);
int MessageHandler_keepalive(void);
int MessageHandler_downside(Message data);

#endif /* SOURCE_HANDLER_MESSAGEHANDLER_H_ */
