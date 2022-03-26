#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        3       /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
INT8U          TaskData[3];                      /* Parameters to pass to each task               */
OS_EVENT     *RandomSem;
INT16U        tick_id;
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void PrintInfo(INT8U* start);
void printinteger(INT8U x, INT8U y, void *number, INT8U size);
void  Task(void *data);                       /* Function prototypes of tasks                  */
void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{
//    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

//    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
//    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    RandomSem   = OSSemCreate(1);                          /* Random number semaphore                  */
//    OSTimeSet(0);
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    TaskData[0] = 13;
	OSTaskCreate(Task, (void *)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 1);
	TaskData[1] = 36;
	OSTaskCreate(Task, (void *)&TaskData[1], &TaskStk[1][TASK_STK_SIZE - 1], 2);
	TaskData[2] = 49;
	OSTaskCreate(Task, (void *)&TaskData[2], &TaskStk[2][TASK_STK_SIZE - 1], 3);

    OSStart();                                             /* Start multitasking                       */
}

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;
    INT16U     order = 0;
    INT8U      start = 0;
    pdata = pdata;                                         /* Prevent compiler warning                 */

    // TaskStartDispInit();                                   /* Initialize the display                   */

//    OS_ENTER_CRITICAL();
//    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
//    // PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate   OS_TICKS_PER_SEC -> 1sec 200 ticks  */
//    PC_SetTickRate(200);
//    OS_EXIT_CRITICAL();

//    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

//    TaskStartCreateTasks();                                /* Create all the application tasks         */

    for (;;) {
        // TaskStartDisp();                                  /* Update the display                       */
        PrintInfo(&start);


        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDlyHMSM(0, 0, 3, 0);                         /* Wait one second                          */
//        OSTimeDly(200);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    // work_time, periodic_time
    // {t1(1,3), t2(3,6)}
    TaskData[0] = 13;
    OSTaskCreate(Task, (void *)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 1);
    TaskData[1] = 36;
    OSTaskCreate(Task, (void *)&TaskData[1], &TaskStk[1][TASK_STK_SIZE - 1], 2);

    // {t3(4,9)}
    TaskData[2] = 49;
//    OSTaskCreate(Task, (void *)&TaskData[2], &TaskStk[2][TASK_STK_SIZE - 1], 3);
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void  Task (void *data)
{
#if OS_CRITICAL_METHOD == 3
                              /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    INT8U  x = 30;

    INT8U  work_time = *(INT8U *)data / 10;
    INT8S  periodic_time = *(INT8U *)data % 10;

    INT32U start;
    INT32U end;
    // think
    INT16S todelay;

    char remain;
    char print = 'a';

    INT32U a;

    // init TCB counter
    OS_ENTER_CRITICAL();
    OSTCBCur->computeticks = work_time;
    OS_EXIT_CRITICAL();
    start = 0;
    while(1){


        OS_ENTER_CRITICAL();
        remain = OSTCBCur->computeticks;
        OS_EXIT_CRITICAL();
        while(1){
            OS_ENTER_CRITICAL();
            todelay = periodic_time - (OSTimeGet() - start);
            if(todelay < 0){
            	OSTCBCur->violate = 1;
//            	printf("45\n");
            }else
            	OSTCBCur->violate = 0;
            remain = OSTCBCur->computeticks;
            if(remain <= 0){
//            	OS_ENTER_CRITICAL();
            	TimeTick[BufferIndex] = OSTimeGet();
//            	OS_EXIT_CRITICAL();
            	break;
            }
            OS_EXIT_CRITICAL();
        }
        // compute delay time
        end = OSTimeGet();
//        if ( end > start + periodic_time ) {
//        	printf("time: %d, task: %d violates\n", start+periodic_time, OSTCBCur->OSTCBPrio);
//        }

        todelay = periodic_time - (end - start);
        start = start + periodic_time;
//        OS_ENTER_CRITICAL();
        OSTCBCur->computeticks = work_time;

        if( todelay < 0 ){
            // show deadline violations
//            OS_ENTER_CRITICAL();
//            TimeTick[BufferIndex] = OSTime;
//            event[BufferIndex] = 'c';
//            to[BufferIndex] = OSTCBCur->OSTCBPrio;
//            from[BufferIndex] = OSTCBCur->OSTCBPrio;
//            v[BufferIndex] = 'v';
//            BufferIndex++;
//            BufferIndex %= 512;
            OS_EXIT_CRITICAL();
            start = end;
        }
        else{
        	OS_EXIT_CRITICAL();
            OSTimeDly(todelay);
        }


    }
}

void PrintInfo(INT8U *start){
#if OS_CRITICAL_METHOD == 3
                              /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char temp[100];
    INT16U i;

    OS_ENTER_CRITICAL();
//    printf("\n");
    for(i = 0; i < BufferIndex-1; i++){
    	memset(temp,'\0',sizeof(temp));
//         sprintf(temp, "%d\t", tick_id);
        // tick_id++ ;
    	sprintf(temp, "%10d    ", TimeTick[i]);
//    	printf("%ld    ", TimeTick[i]);
        if(event[i] == 'p'){
            sprintf(temp + 14, "Preempt  ");
//        	printf("Preempt     ");
        }
        else{
            sprintf(temp + 14, "Complete ");
//        	printf("Complete    ");
        }
        sprintf(temp + 23, "%5u    ", from[i]);
//        printf("%u    ", from[i]);
        sprintf(temp + 32, "%5u    ", to[i]);
//        printf("%u    ", to[i]);
//        printf("%d",strlen(temp));
        if (v[i]=='v') {
            sprintf(temp + 41, "%c", v[i]);
        }
        printf("%s\n",temp);
//        printf("\n");
    }
    BufferIndex = 0;
    OS_EXIT_CRITICAL();
}

