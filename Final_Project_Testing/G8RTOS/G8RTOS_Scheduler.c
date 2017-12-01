/*
 * G8RTOS_Scheduler.c
 */

/*********************************************** Dependencies and Externs *************************************************************/

//#include <stdint.h>
//#include "msp.h"
//#include "G8RTOS.h"
#include "G8RTOS_Scheduler.h"
//#include "G8RTOS_Structures.h"
//#include <driverlib.h>
//#include "Periodic_Events.h"

//#include "G8RTOS_Structures.h"

//#include "Threads.h"

/*
 * G8RTOS_Start exists in asm
 */
extern void G8RTOS_Start();

/* System Core Clock From system_msp432p401r.c */
extern uint32_t SystemCoreClock;

/*
 * Pointer to the currently running Thread Control Block
 */
extern tcb_t * CurrentlyRunningThread;
extern int32_t * contextswitchflag;


int32_t temp;
//  static uint32_t traversCounter;
/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Defines ******************************************************************************/

/* Status Register with the Thumb-bit Set */
#define THUMBBIT 0x01000000

/*********************************************** Defines ******************************************************************************/


/*********************************************** Data Structures Used *****************************************************************/

/* Thread Control Blocks
 *  - An array of thread control blocks to hold pertinent information for each thread
 */
static tcb_t threadControlBlocks[MAX_THREADS];
static PE_t PE_Table[MAX_P_EVENTS];


/* Thread Stacks
 *  - An array of arrays that will act as invdividual stacks for each thread
 */
static int32_t threadStacks[MAX_THREADS][STACKSIZE];


/*********************************************** Data Structures Used *****************************************************************/


/*********************************************** Private Variables ********************************************************************/

/*
 * Current Number of Threads currently in the scheduler
 */
static uint32_t NumberOfThreads;
static uint32_t numberOfevents;


/*********************************************** Private Variables ********************************************************************/


/*********************************************** Private Functions ********************************************************************/

void switchContex(){

    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; // Set pending SysTick bit
}

/*
 * Initializes the Systick and Systick Interrupt
 * The Systick interrupt will be responsible for starting a context switch between threads
 * Param "numCycles": Number of cycles for each systick interrupt
 */
static void InitSysTick(uint32_t numCycles)
{
    SysTick_Config(numCycles);
    //MAP_SysTick_enableModule();
    //MAP_SysTick_setPeriod(1000);
    //MAP_SysTick_enableInterrupt();
}

/*
 * Chooses the next thread to run.
 * Lab 2 Scheduling Algorithm:
 *  - Simple Round Robin: Choose the next running thread by selecting the currently running thread's next pointer
 */
void G8RTOS_Scheduler()
{
    uint16_t max = 256;
    tcb_t* Ptr;
    tcb_t *higher_priority_Ptr;
    higher_priority_Ptr = CurrentlyRunningThread->next_tcb_p;
    Ptr = CurrentlyRunningThread;
    do{
        Ptr = Ptr->next_tcb_p;
        if (Ptr->blocked == 0 && Ptr->isSleep==0 && (uint16_t)Ptr->Priority < max ){
            max = Ptr->Priority;
            higher_priority_Ptr = Ptr;
        }

    } while(CurrentlyRunningThread != Ptr);
    CurrentlyRunningThread = higher_priority_Ptr;
}
//****************************************************************************************************
void G8RTOS_OS_Sleep(uint32_t miliSeconds){
    int sys_status;
    sys_status = StartCriticalSection();
    CurrentlyRunningThread->isSleep = 1;
    CurrentlyRunningThread->sleepCnt = miliSeconds + SystemTime;
    switchContex();
    EndCriticalSection(sys_status);
}
void SysTick_Handler()
{
    int32_t sys_status;
    sys_status=StartCriticalSection();

    SystemTime++;

    for (int  ii=0; ii < numberOfevents; ii++){
        //temp = PE_Table[ii].Execute_Time;
        if (PE_Table[ii].Execute_Time == SystemTime){
            PE_Table[ii].Execute_Time = SystemTime + PE_Table[ii].Period;
            (*PE_Table[ii].Handler)();
        }

    }

    tcb_t *tempTcb = CurrentlyRunningThread;
    for (int  ii=0; ii <NumberOfThreads; ii++)
    {
        if(tempTcb->isSleep == 1)
        {
            if (tempTcb->sleepCnt <= SystemTime )
            {
                tempTcb->isSleep = 0;
                tempTcb->sleepCnt = 0;
            }
        }
        tempTcb = tempTcb->next_tcb_p;
    }

   // Set pending SysTick bit
    EndCriticalSection(sys_status);
    switchContex();
}


/*********************************************** Private Functions ********************************************************************/



/*********************************************** Public Variables *********************************************************************/

/* Holds the current time for the whole System */
uint32_t SystemTime;

/*********************************************** Public Variables *********************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Sets variables to an initial state (system time and number of threads)
 * Enables board for highest speed clock and disables watchdog
 */
void G8RTOS_Init()
{
    SystemTime =0;
    NumberOfThreads =0 ; // Number of Threads is equal to 0
    numberOfevents = 0;
    IDCounter =0;
    BSP_InitBoard(); //Initialize all the hardware

    uint32_t newVTORTable = 0x20000000;
    memcpy((uint32_t *)newVTORTable, (uint32_t *)SCB->VTOR, 57*4);  // 57 interrupt vectors to copy
    SCB->VTOR = newVTORTable;
}

/*
 * Starts G8RTOS Scheduler
 *  - Initializes the Systick
 *  - Sets Context to first thread
 * Returns: Error Code for starting scheduler. This will only return if the scheduler fails
 */
int G8RTOS_Launch()
{
    /* Implement this */
    InitSysTick(ClockSys_GetSysFreq() / 1000);
    CurrentlyRunningThread =&threadControlBlocks[0];
    G8RTOS_Scheduler();
    NVIC_SetPriority(SysTick_IRQn,7);
    NVIC_SetPriority(PendSV_IRQn,7);
    G8RTOS_Start();
    return 0;
}


/*
 * Adds threads to G8RTOS Scheduler
 *  - Checks if there are stil available threads to insert to scheduler
 *  - Initializes the thread control block for the provided thread
 *  - Initializes the stack for the provided thread to hold a "fake context"
 *  - Sets stack tcb stack pointer to top of thread stack
 *  - Sets up the next and previous tcb pointers in a round robin fashion
 * Param "threadToAdd": Void-Void Function to add as preemptable main thread
 * Returns: Error code for adding threads
 */
int32_t G8RTOS_AddThread(void(*threadToAdd)(void), char* name, uint8_t Priority)
{

    int32_t sys_status;
    sys_status = StartCriticalSection();
    int deadThreadFound =0;

    if(NumberOfThreads >= MAX_THREADS){
        return -1;
    }

    int32_t  newStackPointer;
    newStackPointer = &threadStacks[NumberOfThreads][STACKSIZE-16];
    if(NumberOfThreads == 0){
        newStackPointer = &threadStacks[0][STACKSIZE-16];
        threadControlBlocks[0].next_tcb_p = &threadControlBlocks[0];
        threadControlBlocks[0].prev_tcb_p = &threadControlBlocks[0];
        threadControlBlocks[0].tcb_stack_p = newStackPointer;
        threadControlBlocks[0].alive = true;
        threadControlBlocks[0].Priority = Priority;
        threadControlBlocks[0].blocked = 0;
        threadControlBlocks[0].id =(threadId_t) ((IDCounter << 16));
        threadControlBlocks[0].isSleep = 0;
        threadControlBlocks[0].sleepCnt =0;
        *threadControlBlocks[0].threadName = *name;
        threadStacks[0][STACKSIZE-2] = threadToAdd;
        threadStacks[0][STACKSIZE-1] = 0x01000000;   //
        threadStacks[0][STACKSIZE-3] = 0x14141414;   // R14
        threadStacks[0][STACKSIZE-4] = 0x12121212;   // R12
        threadStacks[0][STACKSIZE-5] = 0x03030303;   // R3
        threadStacks[0][STACKSIZE-6] = 0x02020202;   // R2
        threadStacks[0][STACKSIZE-7] = 0x01010101;   // R1
        threadStacks[0][STACKSIZE-8] = 0x00000000;   // R0
        threadStacks[0][STACKSIZE-9] = 0x11111111;   // R11
        threadStacks[0][STACKSIZE-10] = 0x10101010;  // R10
        threadStacks[0][STACKSIZE-11] = 0x09090909;  // R9
        threadStacks[0][STACKSIZE-12] = 0x08080808;  // R8
        threadStacks[0][STACKSIZE-13] = 0x07070707;  // R7
        threadStacks[0][STACKSIZE-14] = 0x06060606;  // R6
        threadStacks[0][STACKSIZE-15] = 0x05050505;  // R5
        threadStacks[0][STACKSIZE-16] = 0x04040404;
    }
    else{
        int i;
        for (i =0; i< MAX_THREADS ; i++){
            if (threadControlBlocks[i].alive == 0){
                break;
            }
        }
        newStackPointer = &threadStacks[i][STACKSIZE-16];
        threadControlBlocks[i].tcb_stack_p = newStackPointer;
        threadControlBlocks[i].alive = true;
        threadControlBlocks[i].Priority = Priority;
        threadControlBlocks[i].blocked = 0;
        threadControlBlocks[i].id = (IDCounter << 16);
        threadControlBlocks[i].isSleep = 0;
        threadControlBlocks[i].sleepCnt =0;
        *threadControlBlocks[i].threadName = *name;
        threadStacks[i][STACKSIZE-2] = threadToAdd;
        threadStacks[i][STACKSIZE-1] = 0x01000000;   //
        threadStacks[i][STACKSIZE-3] = 0x14141414;   // R14
        threadStacks[i][STACKSIZE-4] = 0x12121212;   // R12
        threadStacks[i][STACKSIZE-5] = 0x03030303;   // R3
        threadStacks[i][STACKSIZE-6] = 0x02020202;   // R2
        threadStacks[i][STACKSIZE-7] = 0x01010101;   // R1
        threadStacks[i][STACKSIZE-8] = 0x00000000;   // R0
        threadStacks[i][STACKSIZE-9] = 0x11111111;   // R11
        threadStacks[i][STACKSIZE-10] = 0x10101010;  // R10
        threadStacks[i][STACKSIZE-11] = 0x09090909;  // R9
        threadStacks[i][STACKSIZE-12] = 0x08080808;  // R8
        threadStacks[i][STACKSIZE-13] = 0x07070707;  // R7
        threadStacks[i][STACKSIZE-14] = 0x06060606;  // R6
        threadStacks[i][STACKSIZE-15] = 0x05050505;  // R5
        threadStacks[i][STACKSIZE-16] = 0x04040404;

        int j;
        j=i-1;

//        while (i!=j){
//            if (threadControlBlocks[j].alive == 1) break;
//
//            j--;
//            if (j < 0 ) j= MAX_THREADS -1;
//        }

        tcb_t* temp_ptr;
        threadControlBlocks[i].next_tcb_p = threadControlBlocks[j].next_tcb_p;
        temp_ptr =  threadControlBlocks[i].next_tcb_p;
        temp_ptr->prev_tcb_p = &threadControlBlocks[i];
        threadControlBlocks[i].prev_tcb_p = &threadControlBlocks[j];
        threadControlBlocks[j].next_tcb_p = &threadControlBlocks[i] ;
    }

    NumberOfThreads++;
    IDCounter++;
    EndCriticalSection(sys_status);
    return 0;

}

/*********************************************** Public Functions *********************************************************************/

    /*********************************************** Public Functions *********************************************************************/

threadId_t G8RTOS_GetThreadId(){

    return CurrentlyRunningThread->id;
}
/*********************************************** Public Functions *********************************************************************/


int32_t G8RTOS_KillSelf() {
    G8RTOS_KillThread(CurrentlyRunningThread->id);
    return 0;
}/*********************************************** Public Functions *********************************************************************/


int32_t G8RTOS_KillThread(threadId_t id){

    //  -   Enter a critical section
    int32_t sys_status;
    sys_status = StartCriticalSection();
    if (NumberOfThreads == 1) return -1;
    int i;
    for (i =0 ; i< MAX_THREADS ; i++){
        if  (threadControlBlocks[i].id == id) break;

    }

    if (i == MAX_THREADS ) return -1;
    threadControlBlocks[i].alive = 0;
    tcb_t * next_ptr;
    tcb_t * prev_ptr;
    next_ptr =  threadControlBlocks[i].next_tcb_p;
    prev_ptr =  threadControlBlocks[i].prev_tcb_p;
    next_ptr->prev_tcb_p = prev_ptr;
    prev_ptr->next_tcb_p = next_ptr;

    if  (&threadControlBlocks[i] == CurrentlyRunningThread){
        switchContex();
    }

    NumberOfThreads--;
    //   -   Return appropriate error code if there's only one thread running
    //     -   Search for thread with the same threadId
    //   -   Return error code if the thread does not exist
    //   -   Set the threads isAlive bit to false
    //   -   Update thread pointers
    //   -   If thread being killed is the currently running thread, we need to context switch once critical section is ended
    //   -   Decrement number of threads
    EndCriticalSection(sys_status);
    //  -   End critical section
    return 0;

}


void G8RTOS_AddAPeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, IRQn_Type IRQn){
    int32_t sys_status;

    sys_status = StartCriticalSection();
    if (IRQn >= PORT6_IRQn  || IRQn <= PSS_IRQn) return -1;
    if (priority > 6) return -1;
    NVIC_SetVector(IRQn, AthreadToAdd);
    NVIC_SetPriority(IRQn, priority);
    NVIC_EnableIRQ(IRQn);
    EndCriticalSection(sys_status);
    ///-   Verify the IRQn is less than the last exception (PSS_IRQn) and greater than last acceptable user IRQn (PORT6_IRQn), or else return appropriate error
    //-   Verify priority is not greater than 6, the greatest user priority number, or else return appropriate error
    //-   Use the following core_cm4 library functions to initialize the NVIC registers:
    //o   __NVIC_SetVector
    //o   __NVIC_SetPriority
    //o   NVIC_EnableIRQ
}

/*
 * Currently running thread kills all other threads
 * Returns: Error Code for Removing Threads
 */
int G8RTOS_KillAllOtherThreads()
{
    /* Critical Section */
    int32_t PrimaskState = StartCriticalSection();

    /* Check if this is last thread (we cannot have all threads killed in this scheduler */
    if (NumberOfThreads == 1)
    {
        EndCriticalSection(PrimaskState);
        return -1;
    }

    tcb_t * threadPtr = CurrentlyRunningThread;

    for(int i = 1; i < NumberOfThreads; i++)
    {
        /* Set alive bit of next TCB to false*/
        threadPtr->next_tcb_p->alive= false;

        threadPtr = threadPtr->next_tcb_p;
    }

    /* Update Number of Threads */
    NumberOfThreads = 1;

    /* Set currently running thread's next TCB to itself */
    CurrentlyRunningThread->next_tcb_p = CurrentlyRunningThread;

    /* Set currently running thread's previous TCB to itself */
    CurrentlyRunningThread->prev_tcb_p = CurrentlyRunningThread;

    EndCriticalSection(PrimaskState);
    return -1;
}
