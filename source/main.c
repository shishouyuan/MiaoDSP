/*
 *  ======== main.c ========
 */

#include <xdc/std.h>

#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/utils/Load.h>

#include <DeviceHeaders/include/DSP2803x_Device.h>

#include "core/Core.h"
#include "module/led/led.h"
#include "handler/MessageHandler.h"
#include "module/transfer/Transfer.h"
#include "handler/KeyHandler.h"

Memory_Stats ms;

void loadCall(){
    Memory_getStats(NULL, &ms);

    System_printf("Heap Used: %u %%, CPU Load: %u %%\n",(unsigned int)((float)(ms.totalSize-ms.totalFreeSize)/(ms.totalSize)*100),(unsigned int) Load_getCPULoad());
    System_flush();
}

Int main()
{ 

    EALLOW;
    SysCtrlRegs.XCLK.bit.XCLKOUTDIV=2;
    EDIS;

    coreInit();

    messageHandlerInit();
    ledInit();
    KeyHandlerInit();
    corePutWork(CORE_WORK_PRIORITY_HIGH, Transfer_start,NULL,NULL);

    //hello/
    BIOS_start();    /* does not return */
    return(0);
}
