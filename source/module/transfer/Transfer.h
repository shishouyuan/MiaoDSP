/*
 * TransferModule.h
 *
 *  Created on: 2018��11��7��
 *      Author: Shouyuan
 */

#ifndef SOURCE_SCI_TRANSFERMODULE_H_
#define SOURCE_SCI_TRANSFERMODULE_H_

#include "Message.h"
#include "../sci/Sci.h"

//��ʱ������
#define TRANSFER_KEEP_ALIVE_TIMEOUT 2000

void Transfer_start(CoreArg arg);
int Transfer_init();


#endif /* SOURCE_SCI_TRANSFERMODULE_H_ */
