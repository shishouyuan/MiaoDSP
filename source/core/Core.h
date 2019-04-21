/*
 * Core.h
 *
 *  Created on: 2018Äê11ÔÂ7ÈÕ
 *      Author: Shouyuan
 */

#ifndef SOURCE_CORE_CORE_H_
#define SOURCE_CORE_CORE_H_



#define CORE_WORK_TASK_PRIORITY_COUNT 3
#define CORE_WORK_TASK_STACK_SIZE 256

#define CORE_WORK_TASK_PRIORITY_LOW 1
#define CORE_WORK_TASK_PRIORITY_NORMAL 2
#define CORE_WORK_TASK_PRIORITY_HIGH 3

typedef void (*CleanFunction)(void *);
typedef unsigned long CoreArg;
typedef void (*HandleFunction)(CoreArg);
typedef enum
{
    CORE_WORK_PRIORITY_LOW=0
#if CORE_WORK_TASK_PRIORITY_COUNT>=2
    ,CORE_WORK_PRIORITY_NORMAL=1
#if CORE_WORK_TASK_PRIORITY_COUNT==3
    ,CORE_WORK_PRIORITY_HIGH=2
#endif
#endif
}CoreWorkPriority;

int coreInit();
int corePutWork(CoreWorkPriority priority, HandleFunction handleFunction, CoreArg arg, CleanFunction cleanFunction);

#endif /* SOURCE_CORE_CORE_H_ */
