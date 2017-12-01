/*
 * G8RTOS_Structure.h
 *
 *  Created on: Jan 12, 2017
 *      Author: Raz Aloni
 */
#include "G8RTOS_Semaphores.h"
#ifndef G8RTOS_STRUCTURES_H_
#define G8RTOS_STRUCTURES_H_
#include <stdint.h>
#include "msp.h"
#include "G8RTOS_Semaphores.h"
#include "G8RTOS.h"



#include "stdbool.h"

//#include "stdbool.h"


/*********************************************** Data Structure Definitions ***********************************************************/

/*
 *  Thread Control Block:
 *      - Every thread has a Thread Control Block
 *      - The Thread Control Block holds information about the Thread Such as the Stack Pointer, Priority Level, and Blocked Status
 *      - For Lab 2 the TCB will only hold the Stack Pointer, next TCB and the previous TCB (for Round Robin Scheduling)
 */
typedef  uint32_t threadId_t;
/* Create tcb struct here */
typedef int32_t semaphore_t;
  struct PEV {

    void (*Handler)(void);
    uint32_t Execute_Time;
    uint32_t Current_Time;
    uint32_t Period;
   struct PEV *Previous_P_Event;
   struct PEV *Next_P_Event;

} ;
typedef struct PEV PE_t;

 struct tcb {

    int32_t *tcb_stack_p;
    struct tcb *next_tcb_p;
    struct tcb *prev_tcb_p;
    semaphore_t* blocked;
   volatile uint32_t sleepCnt;
    int isSleep;
    bool alive ;
    uint8_t Priority;
    threadId_t id;
    char threadName[16];

    } ;

    typedef struct tcb tcb_t;








/*********************************************** Data Structure Definitions ***********************************************************/


/*********************************************** Public Variables *********************************************************************/

tcb_t * CurrentlyRunningThread;
int32_t * contextswitchflag;
static  uint16_t IDCounter;
/*********************************************** Public Variables *********************************************************************/




#endif /* G8RTOS_STRUCTURES_H_ */
