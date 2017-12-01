/*
 * G8RTOS_Semaphores.c
 */

/*********************************************** Dependencies and Externs *************************************************************/

#include <stdint.h>
#include "msp.h"
#include "G8RTOS_Semaphores.h"
//#include "G8RTOS_CriticalSection.h"
//#include "G8RTOS_Structures.h"

/*********************************************** Dependencies and Externs *************************************************************/

/*********************************************** Public Functions *********************************************************************/

/*
 * Initializes a semaphore to a given value
 * Param "s": Pointer to semaphore
 * Param "value": Value to initialize semaphore to
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_InitSemaphore(semaphore_t *s, int32_t value)
{
    /* Implement this */
    int32_t sys_status= StartCriticalSection();
    (*s) = value;
    EndCriticalSection(sys_status);
}

/*
 * Waits for a semaphore to be available (value greater than 0)
 *  - Decrements semaphore when available
 *  - Spinlocks to wait for semaphore
 * Param "s": Pointer to semaphore to wait on
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_WaitSemaphore(semaphore_t *s)
{
    /* Implement this */
    int32_t sys_status= StartCriticalSection();
    (*s) = (*s) - 1;

    if ((*s) < 0)
    {
        CurrentlyRunningThread->blocked = s;
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
    // sys_status=StartCriticalSection();    }
    EndCriticalSection(sys_status);

}

/*
 * Signals the completion of the usage of a semaphore
 *  - Increments the semaphore value by 1
 * Param "s": Pointer to semaphore to be signalled
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_SignalSemaphore(semaphore_t *s)
{

    /* Implement this */
    int32_t sys_status= StartCriticalSection();

    tcb_t* ptr;
    ptr = (tcb_t*) CurrentlyRunningThread;
    (*s) = (*s) + 1;
    int counter =0;
    if ((*s) <= 0)
    {

        ptr = ptr->next_tcb_p;

        while (ptr->blocked != s)
        {
            ptr = ptr->next_tcb_p;
           // counter++;
           // if(counter == 30) break;
        }

        ptr->blocked = 0;
    }

    EndCriticalSection(sys_status);

}

/*********************************************** Public Functions *********************************************************************/

