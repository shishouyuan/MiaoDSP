/*
 * TransferModule.h
 *
 *  Created on: 2018年11月7日
 *      Author: Shouyuan
 */

#ifndef SOURCE_SCI_TRANSFERMODULE_H_
#define SOURCE_SCI_TRANSFERMODULE_H_

#include "Message.h"
#include "../sci/Sci.h"

//超时，毫秒
#define TRANSFER_KEEP_ALIVE_TIMEOUT 2000

void Transfer_start(CoreArg arg);
int Transfer_init();


#endif /* SOURCE_SCI_TRANSFERMODULE_H_ */
