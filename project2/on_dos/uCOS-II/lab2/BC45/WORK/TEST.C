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
INT8U         start;
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void PrintInfo();
void printinteger(INT8U x, INT8U y, void *number, INT8U size);
void  Task1(void *data);                       /* Function prototypes of tasks                  */
void  Task2 (void *data);
void  Task3 (void *data);
void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);
void another(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{
    // PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */
    
    char temp[100];
    start = 0;
    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    OSTimeSet(0);

    // OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    TaskData[0] = 14;
	OSTaskCreate(Task1, (void *)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 1);
	TaskData[1] = 25;
	OSTaskCreate(Task2, (void *)&TaskData[1], &TaskStk[1][TASK_STK_SIZE - 1], 2);
	TaskData[2] = 210;
	OSTaskCreate(Task3, (void *)&TaskData[2], &TaskStk[2][TASK_STK_SIZE - 1], 3);
    another();


    OSStart();                                             /* Start multitasking                       */
}

void another(void) {
    OS_TCB *ptcb = OSTCBList;
    int start = 5;
    char temp[100];
    while(ptcb->OSTCBPrio == 0 || ptcb->OSTCBPrio == 1 || ptcb->OSTCBPrio == 2 || ptcb->OSTCBPrio == 3){
        // memset(temp,'\0', 100);
        // sprintf(temp, "%u", ptcb->OSTCBPrio);
        // PC_DispStr(start, start, temp, DISP_FGND_YELLOW + DISP_BGND_BLUE);
        start++;
        switch (ptcb->OSTCBPrio) {
            // case 0:
            //     ptcb->deadline = 0;
            //     // PC_DispStr(5, start, temp, DISP_FGND_YELLOW + DISP_BGND_BLUE);
            //     break;
            case 1:
                ptcb->deadline = 4;
                break;
            case 2:
                ptcb->deadline = 5;
                break;
            case 3:
                ptcb->deadline = 10;
                break;
        }
        ptcb = ptcb->OSTCBNext;        
    }
}

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void  Task1 (void *data)
{
    INT8U  work_time = 1;
    INT8S  periodic_time = 4;

    // INT32U start_time;
    INT32U end;
    INT16S todelay;

    INT16S     key;

    char remain;

    // init TCB counter
    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(40);
    OSTCBCur->computeticks = work_time;
    OS_EXIT_CRITICAL();

    // set initail start_time time to 0, for all task come at timetick 0
    // start_time = 0;
    // PC_DispStr(0,0,"454545454545454545454545454545454545154545454545454545", DISP_FGND_YELLOW + DISP_BGND_BLUE);
    while(1){
        // while(start == 30){
            if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
                if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                    PC_DOSReturn();                            /* Return to DOS                            */
                }
            }
        // }

        OS_ENTER_CRITICAL();
        remain = OSTCBCur->computeticks;
        OS_EXIT_CRITICAL();
        while(1){
            OS_ENTER_CRITICAL();
            // todelay = periodic_time - (OSTimeGet() - start_time);
            // todelay = OSTCBCur->deadline - OSTimeGet();
            if( OSTCBCur->deadline - OSTimeGet() <= 0 && OSTCBCur->computeticks > 0){
            	OSTCBCur->violate = 1;
            }
            else{
            	OSTCBCur->violate = 0;
            }
            remain = OSTCBCur->computeticks;
            if(remain <= 0){
            	end = OSTimeGet();
                todelay = OSTCBCur->deadline - end;
            	break;
            }
            OS_EXIT_CRITICAL();
        }
        // compute delay time
        // end = OSTimeGet();
        // todelay = OSTCBCur->deadline - end;
        // start_time = start_time + periodic_time;
        // start_time = OSTCBCur->deadline;
        OSTCBCur->computeticks = work_time;
        // PC_DispChar(40,20,'A'+todelay, DISP_FGND_YELLOW + DISP_BGND_BLUE);
        PrintInfo();

        if( todelay < 0 ){
            // renew deadline when violation
            OSTCBCur->deadline = end + periodic_time;
            OS_EXIT_CRITICAL();
            // start_time = end;
        }
        else{
            // renew deadline no violation
            OSTCBCur->deadline += periodic_time;
        	OS_EXIT_CRITICAL();
            // PC_DispChar(30,10,'A'+todelay, DISP_FGND_YELLOW + DISP_BGND_BLUE);
            
            // while(1){
            //     if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            //         if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
            //             PC_DOSReturn();                            /* Return to DOS                            */
            //         }
            //     }
            // }
            
            OSTimeDly(todelay);
        }
    }
}



void  Task2 (void *data)
{
    INT8U  work_time = 2;
    INT8S  periodic_time = 5;

    // INT32U start_time;
    INT32U end;
    INT16S todelay;

    char remain;

    // init TCB counter
    OS_ENTER_CRITICAL();
    OSTCBCur->computeticks = work_time;
    OS_EXIT_CRITICAL();

    // set initail start_time time to 0, for all task come at timetick 0
    // start_time = 0;

    while(1){
        OS_ENTER_CRITICAL();
        remain = OSTCBCur->computeticks;
        OS_EXIT_CRITICAL();
        while(1){
            OS_ENTER_CRITICAL();
            // todelay = periodic_time - (OSTimeGet() - start_time);
            // todelay = OSTCBCur->deadline - OSTimeGet();
            if( OSTCBCur->deadline - OSTimeGet() <= 0 && OSTCBCur->computeticks > 0){
            	OSTCBCur->violate = 1;
            }
            else{
            	OSTCBCur->violate = 0;
            }
            remain = OSTCBCur->computeticks;
            if(remain <= 0){
            	TimeTick[BufferIndex] = OSTimeGet();
            	break;
            }
            OS_EXIT_CRITICAL();
        }
        // compute delay time
        end = OSTimeGet();
        todelay = OSTCBCur->deadline - end;
        // start_time = start_time + periodic_time;
        // start_time = OSTCBCur->deadline;
        OSTCBCur->computeticks = work_time;
        
        PrintInfo();

        if( todelay < 0 ){
            // renew deadline when violation
            OSTCBCur->deadline = end + periodic_time;
            OS_EXIT_CRITICAL();
            // start_time = end;
        }
        else{
            // renew deadline no violation
            OSTCBCur->deadline += periodic_time;
        	OS_EXIT_CRITICAL();
            OSTimeDly(todelay);
        }
    }
}

void  Task3 (void *data)
{
    INT8U  work_time = 2;
    INT8S  periodic_time = 10;

    // INT32U start_time;
    INT32U end;
    INT16S todelay;

    char remain;

    // init TCB counter
    OS_ENTER_CRITICAL();
    OSTCBCur->computeticks = work_time;
    OS_EXIT_CRITICAL();

    // set initail start_time time to 0, for all task come at timetick 0
    // start_time = 0;

    while(1){
        while(1){
            OS_ENTER_CRITICAL();
            // todelay = periodic_time - (OSTimeGet() - start_time);
            // todelay = OSTCBCur->deadline - OSTimeGet();
            if( OSTCBCur->deadline - OSTimeGet() <= 0 && OSTCBCur->computeticks > 0){
            	OSTCBCur->violate = 1;
            }
            else{
            	OSTCBCur->violate = 0;
            }
            remain = OSTCBCur->computeticks;
            if(remain <= 0){
            	TimeTick[BufferIndex] = OSTimeGet();
            	break;
            }
            OS_EXIT_CRITICAL();
        }
        // compute delay time
        end = OSTimeGet();
        todelay = OSTCBCur->deadline - end;
        // start_time = start_time + periodic_time;
        // start_time = OSTCBCur->deadline;
        OSTCBCur->computeticks = work_time;
        
        PrintInfo();

        if( todelay < 0 ){
            // renew deadline when violation
            OSTCBCur->deadline = end + periodic_time;

            to[BufferIndex] = OSTCBCur->OSTCBPrio;
            from[BufferIndex] = OSTCBCur->OSTCBPrio;
            v[BufferIndex] = 'v';
            BufferIndex++;
            BufferIndex %= 512;

            OS_EXIT_CRITICAL();
            // start_time = end;
        }
        else{
            // renew deadline no violation
            OSTCBCur->deadline += periodic_time;
        	OS_EXIT_CRITICAL();
            OSTimeDly(todelay);
        }
    }
}

void PrintInfo(){
#if OS_CRITICAL_METHOD == 3
                              /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char temp[100];
    INT16U i;

    OS_ENTER_CRITICAL();
    // BufferIndex = 10;
//    printf("\n");
    for(i = 0; i < BufferIndex; i++){
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
        else{
            sprintf(temp + 41, " ");
        }
        // printf("%s\n",temp);
        PC_DispStr(0, start, temp, DISP_FGND_YELLOW + DISP_BGND_BLUE);
        start = start + 1;
        if(start > 30){
            start = 0;
        }
    }
    BufferIndex = 0;

    OS_EXIT_CRITICAL();
}

