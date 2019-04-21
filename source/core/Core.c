/*
 * Core.c
 *
 *  Created on: 2018年11月7日
 *      Author: Shouyuan
 */
#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <stdlib.h>

#include "Core.h"

typedef struct
{
    Queue_Elem elem;
    HandleFunction handleFunction;
    CoreArg arg;
    CleanFunction cleanFunction;
} WorkQueueElement;

Void workTaskFun(UArg arg0, UArg arg1);

Queue_Struct workQueStruct[CORE_WORK_TASK_PRIORITY_COUNT];
Queue_Handle workQueueHd[CORE_WORK_TASK_PRIORITY_COUNT];

Semaphore_Struct workQueSemStruct[CORE_WORK_TASK_PRIORITY_COUNT];
Semaphore_Handle workQueSemHd[CORE_WORK_TASK_PRIORITY_COUNT];

Task_Struct workTaskStruct[CORE_WORK_TASK_PRIORITY_COUNT];
Task_Handle workTaskHd[CORE_WORK_TASK_PRIORITY_COUNT];

#pragma DATA_SECTION(workTaskStack,".taskStack");
char workTaskStack[CORE_WORK_TASK_PRIORITY_COUNT][CORE_WORK_TASK_STACK_SIZE];

#if CORE_WORK_TASK_PRIORITY_COUNT==1
int workTaskPrioriry[1]=
{   CORE_WORK_TASK_PRIORITY_LOW};
#elif CORE_WORK_TASK_PRIORITY_COUNT==2
int workTaskPrioriry[2]=
{   CORE_WORK_TASK_PRIORITY_LOW, CORE_WORK_TASK_PRIORITY_NORMAL};
#elif CORE_WORK_TASK_PRIORITY_COUNT==3
int workTaskPrioriry[3] = { CORE_WORK_TASK_PRIORITY_LOW,
CORE_WORK_TASK_PRIORITY_NORMAL,
                            CORE_WORK_TASK_PRIORITY_HIGH };
#endif

//成功返回0，失败返回1
int coreInitWorkQueueTask()
{
    int i;
    Queue_Params queParam;
    Semaphore_Params semParam;
    Task_Params taskParam;

    for (i = 0; i < CORE_WORK_TASK_PRIORITY_COUNT; i++)
    {
        Queue_Params_init(&queParam);
        Queue_construct(&workQueStruct[i], &queParam);
        workQueueHd[i] = Queue_handle(&workQueStruct[i]);
        if (workQueueHd[i] == NULL)
        {
            return 1;
        }

        Semaphore_Params_init(&semParam);
        semParam.mode = Semaphore_Mode_COUNTING;
        Semaphore_construct(&workQueSemStruct[i], 0, &semParam);
        workQueSemHd[i] = Semaphore_handle(&workQueSemStruct[i]);
        if (workQueSemHd[i] == NULL)
        {
            return 1;
        }

        Task_Params_init(&taskParam);
        taskParam.priority = workTaskPrioriry[i];
        taskParam.stack = workTaskStack[i];
        taskParam.stackSize = sizeof(workTaskStack[i]);
        taskParam.arg0 = i;

        Task_construct(&workTaskStruct[i], workTaskFun, &taskParam, NULL);
        workTaskHd[i] = Task_handle(&workTaskStruct[i]);
        if (workTaskHd[i] == NULL)
        {
            return 1;
        }
    }
    return 0;
}

//成功返回0，失败返回1
int corePutWork(CoreWorkPriority priority, HandleFunction handleFunction,
                CoreArg arg, CleanFunction cleanFunction)
{
    WorkQueueElement *ctask = (WorkQueueElement *) malloc(sizeof(WorkQueueElement));
    if (ctask == NULL)
    {
        return 1;
    }
    ctask->handleFunction = handleFunction;
    ctask->arg = arg;
    ctask->cleanFunction = cleanFunction;

    Queue_put(workQueueHd[priority], (Queue_Elem *) ctask);
    Semaphore_post(workQueSemHd[priority]);
    return 0;
}

Void workTaskFun(UArg arg0, UArg arg1)
{
    int i = (int) arg0;
    while (1)
    {
        Semaphore_pend(workQueSemHd[i], BIOS_WAIT_FOREVER);
        WorkQueueElement *ctask = (WorkQueueElement *) Queue_get(
                workQueueHd[i]);
        if ((Queue_Handle) ctask == workQueueHd[i]) //队列为空则跳过
        {
            continue;
        }

        ctask->handleFunction(ctask->arg);

        //释放空间
        if (ctask->cleanFunction != NULL)
        {
            ctask->cleanFunction((void*) ctask->arg);
        }
        free(ctask);
    }
}

int coreInit()
{

    return coreInitWorkQueueTask();
}
