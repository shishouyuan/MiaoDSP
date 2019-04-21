/*
 * Sci.h
 *
 *  Created on: 2018Äê11ÔÂ8ÈÕ
 *      Author: Shouyuan
 */

#ifndef SOURCE_TRANSFER_SCI_H_
#define SOURCE_TRANSFER_SCI_H_


#include <xdc/std.h>

#include <DeviceHeaders/include/DSP2803x_Device.h>
#include "Sci.h"

#define SCI_TX_INT_NUM 97
#define SCI_RX_INT_NUM 96

typedef void (*SciRxCallback)(char);

int sciIsBusy();
int sciTrySend(char *data, UInt16 length, UInt32 waittime);
int sciSend(char *data, UInt16 length);
int sciInit(SciRxCallback cb);

#endif /* SOURCE_TRANSFER_SCI_H_ */
