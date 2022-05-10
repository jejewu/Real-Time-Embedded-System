/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

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
INT8U         TaskData[3];                      /* Parameters to pass to each task               */
INT16U        order = 0;
INT8U         start = 0;


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
        void PrintInfo();
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
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */
    
    OSTimeSet(0);

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */
    TaskData[0] = 13;
    OSTaskCreate(Task, (void *)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 1);
    TaskData[1] = 36;
    OSTaskCreate(Task, (void *)&TaskData[1], &TaskStk[1][TASK_STK_SIZE - 1], 2);
    // {t3(4,9)}
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
    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    // PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate   OS_TICKS_PER_SEC -> 1sec 200 ticks  */
    PC_SetTickRate(100);
    OS_EXIT_CRITICAL();
    // OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    for (;;) {
        // TaskStartDisp();                                  /* Update the display                       */
        // PrintInfo(&order, &start);

        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        // OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
        OSTimeDly(40);
        while(1){
            if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
                if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                    PC_DOSReturn();                            /* Return to DOS                            */
                }
            }
        }

    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

// static  void  TaskStartCreateTasks (void)
// {
//     // work_time, periodic_time 
//     // {t1(1,3), t2(3,6)}
//     TaskData[0] = 13;
//     OSTaskCreate(Task, (void *)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 1);
//     TaskData[1] = 36;
//     OSTaskCreate(Task, (void *)&TaskData[1], &TaskStk[1][TASK_STK_SIZE - 1], 2);

//     // {t3(4,9)}
//     TaskData[2] = 49;
//     OSTaskCreate(Task, (void *)&TaskData[2], &TaskStk[2][TASK_STK_SIZE - 1], 3);


// }

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void  Task (void *data)
{
    INT16S     key;
    INT8U  x = 0;
    // INT8U  y = *(char*)period-'0';

    INT8U  work_time = *(INT8U *)data / 10;
    INT8S periodic_time = *(INT8U *)data % 10;
    
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

    start = OSTimeGet();

    while(1){
        PrintInfo();
        // printinteger(x, work_time++,&start,32);

        OS_ENTER_CRITICAL();
        remain = OSTCBCur->computeticks;
        OS_EXIT_CRITICAL();
        
        while(remain > 0){
            OS_ENTER_CRITICAL();
            remain = OSTCBCur->computeticks;
            OS_EXIT_CRITICAL();
            // PC_DispChar(x++, work_time + 3, remain, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
        }

        OS_ENTER_CRITICAL();
        // compute delay time
        end = OSTimeGet();
        
        todelay = periodic_time - (end - start);
        start = start + periodic_time;
        
        OSTCBCur->computeticks = work_time;
        
        if( todelay < 0 ){

            while(1){
                if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
                if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                    PC_DOSReturn();                            /* Return to DOS                            */
                }
            }
            }
            // show deadline violations
            // PC_DispChar(x, work_time + 3, 'v', DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
            start = end;

            // OS_ENTER_CRITICAL();
            TimeTick[index] = OSTime;
            event[index] = 'c';
            from[index] = OSTCBCur->OSTCBPrio;
            to[index] = OSTCBCur->OSTCBPrio;
            v[index] = 'v';
            index++;
            index %= 256;
            OS_EXIT_CRITICAL();
        }
        else{
            // printinteger(5, 8+5, &todelay, 32);
            // PC_DispChar(x, work_time + 3, '0'+todelay, DISP_FGND_BLACK + DIS。P_BGND_LIGHT_GRAY);
            // declaration -> void  OSTimeDly (INT16U ticks);
            OS_EXIT_CRITICAL();
            OSTimeDly(todelay);
        }


    }
}

void PrintInfo(){
    char temp[50];
    INT8U i;

    OS_ENTER_CRITICAL();
    for(i = 0; i < index; i++ ){
        memset(temp,'\0',sizeof(temp));
        sprintf(temp, "%d    ", TimeTick[i]);
        if(event[i] == 'p'){
            sprintf(temp + strlen(temp) - 1, "Preempt     ");
        }
        else{
            sprintf(temp + strlen(temp) - 1, "Complete    ");
        }
        sprintf(temp + strlen(temp) - 1, "%u    ", from[i]);
        sprintf(temp + strlen(temp) - 1, "%u   ", to[i]);

        if (v[i]=='v') {
            sprintf(temp + strlen(temp) - 1, "%c", v[i]);
        }
        else{
            sprintf(temp + strlen(temp) - 1, " ", v[i]);
        }

        PC_DispStr(0, start, temp, DISP_FGND_YELLOW + DISP_BGND_BLUE);
        
        start = start + 1;
        if(start > 30){
            start = 0;
        }
        // if((order) >= 0xFFFF){
        //     (order) = 0;
        // }
    }
    index = 0;
    OS_EXIT_CRITICAL();
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDispInit (void)
{
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  0, "                         uC/OS-II, The Real-Time Kernel                         ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr( 0,  1, "                                Jean J. Labrosse                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  2, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  3, "                                    EXAMPLE #1                                  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  5, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  6, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  7, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  8, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  9, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 11, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 14, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 17, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 20, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 21, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 22, "#Tasks          :        CPU Usage:     %                                       ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 23, "#Task switch/sec:                                                               ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           UPDATE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDisp (void)
{
    char   s[80];


    sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */
    PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

#if OS_TASK_STAT_EN > 0
    sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */
    PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
#endif

    sprintf(s, "%5d", OSCtxSwCtr);                                 /* Display #context switches per second */
    PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100); /* Display uC/OS-II's version number    */
    PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    switch (_8087) {                                               /* Display whether FPU present          */
        case 0:
             PC_DispStr(71, 22, " NO  FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 1:
             PC_DispStr(71, 22, " 8087 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 2:
             PC_DispStr(71, 22, "80287 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 3:
             PC_DispStr(71, 22, "80387 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;
    }
}