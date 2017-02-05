/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation, 
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/*
 * Author: 2002-2004 Paolo Gai
 * CVS: $Id: ee_common.h,v 1.7 2006/05/03 05:59:55 pj Exp $
 */

#ifndef __INCLUDE_OO_COMMON_H__
#define __INCLUDE_OO_COMMON_H__

/*************************************************************************
 Kernel Constants
 *************************************************************************/

/* invalid TID */
#define EE_NIL       ((EE_TID)-1)

/* 
The Kernel constants like NIL and other thread statuses are defined
in the following sections: 

- invalid see 13.2.4 
- thread statuses see 13.2.4
*/

/* Moreover, the user must specify (into types.h) in a fashion like 
   #define identifier unique_number
   the following identifiers:
   - The resource identifiers declared with the macro
     DeclareResource (13.4.2.1)
   - The task identifiers declared with the macro TASK(TaskName) (13.2.5)
   - The resource (RES_SCHEDULER) MUST have 
     the maximum ceiling possible (~(~(TYPEPRIO)0 >> 1))

   For the Extended status and ORTI support:
   - EE_MAX_TASK maximum number of tasks (used in TaskActivate
   - EE_MAX_RESOURCE maximum number of resources
   - EE_MAX_ALARM maximum number of alarms

   For TASK/ALARM Autostart support: 
   - EE_MAX_APPMODE maximum number of Application modes (typically
     >0, because there is always the mode OSDEFAULTAPPMODE.
*/


/*************************************************************************
 Kernel Types
 *************************************************************************/

/* priority type */
#ifndef EE_TYPEPRIO
#define EE_TYPEPRIO EE_UREG
#endif

/* status type */
/* the status type, that usually was an used configurable type, is set
   by the OO Standard (Bindings specification, paragraph 3.2) to be
   unsigned char. */
#ifndef EE_TYPESTATUS
#define EE_TYPESTATUS EE_UREG
#endif

/* pending activation type */
#ifndef EE_TYPENACT
#define EE_TYPENACT EE_UREG
#endif

/* boolean type */
#ifndef EE_TYPEBOOL
#define EE_TYPEBOOL EE_UREG
#endif


/* event mask type */
#ifndef EE_TYPEEVENTMASK
#define EE_TYPEEVENTMASK EE_UREG
#endif

/* pair type (signed!) */
#ifndef EE_TYPEPAIR
#define EE_TYPEPAIR EE_SREG
#endif

/* application mode type */
#ifndef EE_TYPEAPPMODE
#define EE_TYPEAPPMODE EE_UREG
#endif

/* service id type */
#ifndef EE_TYPEOSSERVICEID
#define EE_TYPEOSSERVICEID EE_UINT8
#endif

/* resource id type */
#ifndef EE_TYPERESOURCE
#define EE_TYPERESOURCE EE_UREG
#endif

/* alarm id type (signed!) */
#ifndef EE_TYPEALARM
#define EE_TYPEALARM EE_SREG
#endif

/* counter id type (signed!) */
#ifndef EE_TYPECOUNTER
#define EE_TYPECOUNTER EE_SREG
#endif

/* counter tick type (signed!) */
#ifndef EE_TYPETICK
#define EE_TYPETICK EE_UREG
#endif

/* notification type (signed!) */
#ifndef EE_TYPENOTIFY
#define EE_TYPENOTIFY EE_UINT8
#endif


/*************************************************************************
 Kernel Variables defined by the application
 *************************************************************************/

/* thread status, all initialized to SUSPENDED */
extern EE_TYPESTATUS EE_th_status[];

/* next: is used for:
   - the stacked queue
   - WaitEvent (?)
   - the ready queue (BCC1, ECC1)
   all initialized with EE_NIL
*/
extern EE_TID EE_th_next[];

/* priorities (NB: they are bit fields!!!) */
extern const EE_TYPEPRIO EE_th_ready_prio[];
extern const EE_TYPEPRIO EE_th_dispatch_prio[];

/* 
 * remaining nact: init= maximum pending activations of a Task 
 * =1 for BCC1 and ECC1, >= 0 for BCC2 and ECC2 
 *
 * all initialized with 1 (ECC2, BCC2: or with a value >1)
 */
extern EE_TYPENACT   EE_th_rnact[];

#ifndef __OO_NO_CHAINTASK__
/* The next task to be activated after a ChainTask. initvalue=all EE_NIL */
extern EE_TID EE_th_terminate_nextask[];
#endif

/* The first stacked task (initvalue = EE_NIL) */
extern EE_TID EE_stkfirst;

/* System ceiling (initvalue = 0) */
extern EE_TYPEPRIO   EE_sys_ceiling;



#if defined(__OO_BCC1__) || defined(__OO_ECC1__)

/* First task in the ready queue (initvalue = EE_NIL) */
extern EE_TID EE_rq_first;

#endif




/*
 * ready queue implementation:
 * - 16 priorities
 * - we use a queue for each priority. Head and tail are stored in an array
 * - each queue contains pairs (Task ID, next)
 * - the number of pairs is the sum of all the possible activations
 *   of all the tasks (in that way, an activation will never fail due
 *   to the lack of a pair)
 * - to know which queue have to be used, a bit mask is used to do a fast
 *   lookup (the 8 bit lookup table is defined into ee_lookup.c)
 */

/* bit mask with 16 or 8 priority levels (Initvalue = 0) */
#if defined(__OO_BCC2__)
extern EE_UINT8  EE_rq_bitmask;
#else
extern EE_UINT16 EE_rq_bitmask;
#endif

/* The following data structure gives the link between a task and its
   priority queue.  The values of this data structure are the same of
   EE_ready_prio, except that they are not stored as bitfields 

   Initvalue: each TID = x such that th_ready_prio[TID]= 1<<x
*/
extern EE_TYPEPRIO EE_rq_link[];

/* The priority queues  (initvalue: all -1; 
   number of elements: 8(BCC2) or 16(ECC2) ) */
extern EE_TYPEPAIR EE_rq_queues_head[];
extern EE_TYPEPAIR EE_rq_queues_tail[];

/* The pairs that are enqueued into the priority queues */
/* initvalue: something like {1,2,3,4,5,...,-1}. 
   the number of elements is equal to the sum of the elements of 
   EE_th_rnact */
extern EE_TYPEPAIR EE_rq_pairs_next[];
/* init value=0 (no init value); the number of elements is equal to the
   sum of the elements of EE_th_rnact */
extern EE_TID      EE_rq_pairs_tid[];

/* a list of unused pairs */
extern EE_TYPEPAIR EE_rq_free; /* pointer to a free pair; initvalue=0 */




/* Event handling */

/* Note: To save memory space, Extended tasks should have the smallest
   number into the task data structures. In that way, the following
   data structures can be sized to exactly the number of the extended
   tasks */
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
/* thread events already active; these events are set using the
   SetEvent primitive. initvalue = 0 */
extern EE_TYPEEVENTMASK EE_th_event_active[];

/* thread wait mask. this is the event mask the task is waiting using 
   WaitEvent. A task IS waiting only if the value in this array IS != 0.
   initvalue = 0 */
extern EE_TYPEEVENTMASK EE_th_event_waitmask[];

/* this structure contains a flag that is 1 if a thread has been
   suspended using EE_hal_stkchange. In that case, the task have to
   be wakened again using the same function. initvalue = 0
*/
extern EE_TYPEBOOL EE_th_waswaiting[];

/* just a flag: this flag is 1 if the task is an extended task, 0 otherwise */
/* Please note that this flag is defined both in STANDARD and in
   EXTENDED status. The need for this flag in standard status is
   because task activations of an extended task clears its pending
   event mask. */
extern EE_TYPEBOOL EE_th_is_extended[];

#endif


/* Resources data structures */

#ifndef __OO_NO_RESOURCES__

/* Note: There is no constraint on the index that is assigned
   RES_SCHEDULER!!! */

extern const EE_TYPEPRIO   EE_resource_ceiling[]; /* resource ceiling */
extern EE_TYPEPRIO   EE_resource_oldceiling[];    /* old resource ceiling */

#ifdef __OO_EXTENDED_STATUS__ 
/* Only in extended status; for each task, we allocate a data
   structure that keeps track of the order in which every task has
   allocated a resource. This is needed to return a meaningful
   E_OS_NOFUNC error in the ReleaseResource call. */

/* This is the last resource that the task has locked. This array
   contains one entry for each task.  Initvalue= all -1. at runtime,
   it points to the first item in the EE_resource_stack data structure */
extern EE_UREG EE_th_resource_last[];
/* this array is used to store a list of resources locked by a
   task. there is one entry for each resource, initvalue = -1. the
   list of resources locked by a task is ended by -1. */
extern EE_UREG EE_resource_stack[];
#endif

#if defined(__OO_EXTENDED_STATUS__) || defined(__OO_ORTI_RES_ISLOCKED__)
/* Only in extended status or when using ORTI with resources; for each
   resource, a flag is allocated to see if the resource is locked or
   not.  Note that this information cannot be easily knew from the
   previous two data structures. initvalue=0
*/
extern EE_TYPEBOOL EE_resource_locked[];
#endif

#endif

/* Alarms Handling */

/* maybe soon or later I will convert them into arrays :-( PJ */

#ifndef __OO_NO_ALARMS__

/* initvalue: depends on the architecture and on the specific implementation */
typedef struct {
  EE_TYPETICK maxallowedvalue;
  EE_TYPETICK ticksperbase;
  EE_TYPETICK mincycle;
} EE_oo_counter_ROM_type;

/* initvalue: {0, -1} */
typedef struct {
  EE_TYPETICK value;              /* current value of the counter */
  EE_TYPEALARM first;         /* first alarm queued on the counter */
} EE_oo_counter_RAM_type;


/* these are the different types of alarm notifications... */
#define EE_ALARM_ACTION_TASK 0
#define EE_ALARM_ACTION_CALLBACK 1
#define EE_ALARM_ACTION_EVENT 2


/* initvalue: {a_valid_counter, a_valid_action, then you must put the correct
   parameters depending on the action } */
typedef struct {
  EE_TYPECOUNTER c;           /* the counter linked to the alarm */

  EE_TYPENOTIFY action;       /* task=0 event=1 callb=2 */
  
  EE_TID TaskID;

  EE_TYPEEVENTMASK Mask;

  EE_ADDR f;
} EE_oo_alarm_ROM_type;

/* initvalue: all zeroes --> no initialization! */
typedef struct {
  EE_TYPEBOOL used;           /* a flag that is 1 when the alarm is armed */
  EE_TYPETICK cycle;          /* cycle for periodic alarms */
  EE_TYPETICK delta;          /* delta expiration time (into a queue!) */
  EE_TYPEALARM next;          /* next alarm in the delta queue */
} EE_oo_alarm_RAM_type;

/* this array contains, for each counter, the characteristics of the counter.
   The initialization value is implementation dependent */
extern const EE_oo_counter_ROM_type EE_counter_ROM[];

/* this is the RAM part of a counter. 
   Initvalue = an array of {0,-1} elements */
extern EE_oo_counter_RAM_type       EE_counter_RAM[];

/* this is the fixed part of the configuration of an alarm
   Initvalue= depends on how the alarm notification have to be configured */
extern const EE_oo_alarm_ROM_type   EE_alarm_ROM[];

/* this part is the variable part of an alarm.  
   Initvalue: all zeroes. Note that setting the next value to 0 and
   not -1 does not give problems because used=0; the next field will
   be set by alarm_insert when inserting the alarm in the queue */
extern EE_oo_alarm_RAM_type         EE_alarm_RAM[];

#endif


/* this is the stub that have to be put into the EE_th_body array */
extern void EE_oo_thread_stub(void);



/***************************************************************************
 * 13.1 Common data types
 ***************************************************************************/

/* 3.2 (Bindings). StatusType is an unsigned char */
#ifndef STATUSTYPEDEFINED
#define STATUSTYPEDEFINED
typedef unsigned char StatusType;
#define E_OK                                 0
#endif

#define E_OS_ACCESS   1
#define E_OS_CALLEVEL 2
#define E_OS_ID       3
#define E_OS_LIMIT    4
#define E_OS_NOFUNC   5
#define E_OS_RESOURCE 6
#define E_OS_STATE    7
#define E_OS_VALUE    8

/* Implementation specific errors */

/* error during StartOS */
#define E_OS_SYS_INIT 9

/* they must start with E_OS_SYS_* */


/***************************************************************************
 * ORTI support
 ***************************************************************************/

#ifdef __OO_ORTI_LASTERROR__
/* a lasterror value that can be used with ORTI */
extern StatusType EE_ORTI_lasterror;
#endif

#ifdef __OO_ORTI_SERVICETRACE__
/* the last OO service called by the application.  SERVICETRACE IDs
   are odd numbers. The LSBit is used as a flag and it is set to 1
   when the servce is entered, to 0 at exit.
*/

#define EE_SERVICETRACE_ACTIVATETASK              2
#define EE_SERVICETRACE_TERMINATETASK             4
#define EE_SERVICETRACE_CHAINTASK                 6
#define EE_SERVICETRACE_SCHEDULE                  8
#define EE_SERVICETRACE_GETTASKID                10
#define EE_SERVICETRACE_GETTASKSTATE             12
#define EE_SERVICETRACE_DISABLEALLINTERRUPTS     14
#define EE_SERVICETRACE_ENABLEALLINTERRUPTS      16
#define EE_SERVICETRACE_SUSPENDALLINTERRUPTS     18
#define EE_SERVICETRACE_RESUMEALLINTERRUPTS      20
#define EE_SERVICETRACE_SUSPENDOSINTERRUPTS      22
#define EE_SERVICETRACE_RESUMEOSINTERRUPTS       24
#define EE_SERVICETRACE_GETRESOURCE              26
#define EE_SERVICETRACE_RELEASERESOURCE          28
#define EE_SERVICETRACE_SETEVENT                 30
#define EE_SERVICETRACE_CLEAREVENT               32
#define EE_SERVICETRACE_GETEVENT                 34
#define EE_SERVICETRACE_WAITEVENT                36
#define EE_SERVICETRACE_GETALARMBASE             38
#define EE_SERVICETRACE_GETALARM                 40
#define EE_SERVICETRACE_SETRELALARM              42
#define EE_SERVICETRACE_SETABSALARM              44
#define EE_SERVICETRACE_CANCELALARM              46
#define EE_SERVICETRACE_GETACTIVEAPPLICATIONMODE 48
#define EE_SERVICETRACE_STARTOS                  50
#define EE_SERVICETRACE_SHUTDOWNOS               52
#define EE_SERVICETRACE_FORCESCHEDULE            54
#define EE_SERVICETRACE_COUNTERTICK              56

#ifdef __OO_SEM__
#define EE_SERVICETRACE_INITSEM                  58
#define EE_SERVICETRACE_WAITSEM                  60
#define EE_SERVICETRACE_TRYWAITSEM               62
#define EE_SERVICETRACE_POSTSEM                  64
#define EE_SERVICETRACE_GETVALUESEM              66
#endif

extern volatile EE_UINT8 EE_ORTI_servicetrace;
#endif

#ifdef __OO_ORTI_PRIORITY__
/* This flag enables the visualization of the current task priority in ORTI
   debuggers. */

/* this variable contains the current task priority and that is the priority
   that is read by the ORTI debugger
   Initvalue: the ready priority set for the task
*/
extern EE_TYPEPRIO EE_ORTI_th_priority[];

/* this variable contains the current task priority saved when a task
   locked a resource. It works because a resource can be
   locked only by one task at a time. 
   InitValue: all 0
*/
extern EE_TYPEPRIO EE_ORTI_resource_oldpriority[];
#endif

#ifdef __OO_ORTI_RUNNINGISR2__
/* this variable stores 0 if no ISR is running, or the address of the ISR stub
   generated for the particular ISR handler 
   Initvalue: 0
*/
extern EE_ADDR EE_ORTI_runningisr2;
#endif

#ifdef __OO_ORTI_ALARMTIME__
/* this variable stores the time until an alarm expires; it is only
   valid if an alarm is running. 
   initvalue: all 0
*/
extern EE_TYPETICK EE_ORTI_alarmtime[];
#endif

#ifdef __OO_ORTI_RES_LOCKER_TASK__
/* This is task that has currently locked the resource. 
   Initvalue: all 0
*/
extern EE_UREG EE_ORTI_res_locker[];
#endif

/***************************************************************************
 * Autostart Features inside StartOS()
 ***************************************************************************/

/* Note: Autostart uses the symbol EE_MAX_APPMODE that represents
   the number of different application modes in the system. */

#ifdef __OO_AUTOSTART_TASK__

/* This is the data structure used to store the autostart data for tasks.

   - n contains the number of tasks that have to be automatically
     activated at startup for a given application mode.

   - task contains the list of TIDs that have to be activated for a
     given application mode.
*/

struct EE_oo_autostart_task_type {
  EE_UREG n;
  const EE_TID *task;
};

/* For each valid APPMODE (that ranges from 0 to EE_MAX_APPMODE-1) there must
   be an item in this array with the tasks that are activated at startup. */
extern const struct EE_oo_autostart_task_type EE_oo_autostart_task_data[];

#endif

#ifdef __OO_AUTOSTART_ALARM__

/* This is the data structure used to store the autostart data for alarms.

   - n contains the number of alarms that have to be automatically
     set at startup for a given application mode.

   - alarm contains the list of Alarm IDs that have to be activated for a
     given application mode.
*/

struct EE_oo_autostart_alarm_type {
  EE_UREG n;
  const EE_TYPEALARM *alarm;
};

/* For each valid APPMODE (that ranges from 0 to EE_MAX_APPMODE-1) there must
   be an item in this array with the tasks that are activated at startup. */
extern const struct EE_oo_autostart_alarm_type EE_oo_autostart_alarm_data[];

/* For each Alarm that is activated there should be an item in these
   arrays that contains the cycle and increment values that have to be
   used when activating a given alarm ID. Note that cycle/increment
   are unique for each alarm activation time. 
   The size of these two arrays is MAXALARM.
*/
extern const EE_TYPETICK EE_oo_autostart_alarm_increment[];
extern const EE_TYPETICK EE_oo_autostart_alarm_cycle[];
#endif


/***************************************************************************
 * 13.1 Common data types
 ***************************************************************************/

/* see top of the file */


/***************************************************************************
 * 13.2 Task management 
 ***************************************************************************/

/* 13.2.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

/* This data type identifies a task. */
typedef EE_TID TaskType;

/* This data type points to a variable of TaskType. */
typedef EE_TID *TaskRefType;

/* This data type identifies the state of a task. */
typedef EE_TYPESTATUS TaskStateType;

/* This data type points to a variable of the data type TaskStateType. */
typedef EE_TYPESTATUS *TaskStateRefType;



/* 13.2.4 Constants                                                        */
/* ----------------------------------------------------------------------- */

/* Constant of data type TaskStateType for task state running. */
#define RUNNING   0

/* Constant of data type TaskStateType for task state waiting. */
#define WAITING   1

/* Constant of data type TaskStateType for task state ready. */
#define READY     2

/* Constant of data type TaskStateType for task state suspended. */
#define SUSPENDED 3


/***************************************************************************
 * 13.4 Resource management 
 ***************************************************************************/

/* 13.4.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

#ifndef __OO_NO_RESOURCES__
typedef EE_TYPERESOURCE ResourceType;
#endif


/***************************************************************************
 * 13.5 Event control 
 ***************************************************************************/

/* 13.5.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

/* Data type of the event mask. */
typedef EE_TYPEEVENTMASK EventMaskType;

/* Reference to an event mask. */
typedef EE_TYPEEVENTMASK *EventMaskRefType;



/***************************************************************************
 * Semaphores
 ***************************************************************************/

/* Semaphores are not part of the OSEK API, please refer to the EE
   reference manual */

/* Constants                                                               */
/* ----------------------------------------------------------------------- */

/* the maximum unsigned integer */
#define EE_MAX_SEM_COUNTER ((unsigned int)-1)

/* Data types                                                              */
/* ----------------------------------------------------------------------- */

#ifdef __OO_SEM__

struct EE_TYPESEM {
  unsigned int count;

  EE_TID    first;
  EE_TID    last;

};

/* Data type of the event mask. */
typedef struct EE_TYPESEM  SemType;

/* Reference to an event mask. */
typedef SemType *SemRefType;

#endif


/***************************************************************************
 * 13.6 Alarms 
 ***************************************************************************/

/* 13.6.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

#ifndef __OO_NO_ALARMS__

/* This data type represents count values in ticks. */
typedef EE_TYPETICK TickType;

/* This data type points to the data type TickType. */
typedef EE_TYPETICK *TickRefType;

/* A structure for storage of counter characteristics. */
typedef struct {
  TickType maxallowedvalue; /* Maximum possible allowed count value in
                               ticks */
  TickType ticksperbase;    /* Number of ticks required to reach a
			       counter-specific (significant) unit. */
#ifdef __OO_EXTENDED_STATUS__
  TickType mincycle; /* Smallest allowed value for the cycle-parameter
                        of SetRelAlarm/SetAbsAlarm) (only for systems
                        with extended status) */
#endif
} AlarmBaseType;


/* This data type points to the data type AlarmBaseType. */
typedef AlarmBaseType *AlarmBaseRefType;

/* This data type represents an alarm object. */
typedef EE_TYPEALARM AlarmType;

/* This is an additional data type that represents a counter object. */
typedef EE_TYPECOUNTER CounterType;


#endif


/***************************************************************************
 * 13.7 Operating system execution control 
 ***************************************************************************/

/* 13.7.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

/* This data type represents the application mode. */
typedef EE_TYPEAPPMODE AppModeType;


/***************************************************************************
 * 13.8 Hook routines 
 ***************************************************************************/


/* 13.8.1 Data Types                                                       */
/* ----------------------------------------------------------------------- */

/* This data type represents the identification of system services. */
typedef EE_TYPEOSSERVICEID OSServiceIdType;


/* 13.8.3 Constants                                                        */
/* ----------------------------------------------------------------------- */

#ifdef __OO_HAS_ERRORHOOK__
#ifndef __OO_ERRORHOOK_NOMACROS__

/* unique identifier of system service xx */
#define OSServiceId_ActivateTask     1
#define OSServiceId_TerminateTask    2
#define OSServiceId_ChainTask        3
#define OSServiceId_Schedule         4
/* GetTaskID never returns an error */
#define OSServiceId_GetTaskState     5
/* DisableAllInterrupts, EnableAllInterrupts, SuspendAllInterrupts,
   ResumeAllInterrupts, SuspendOSInterrupts, ResumeOSInterrupts never
   return an error */
#define OSServiceId_GetResource      6
#define OSServiceId_ReleaseResource  7
#define OSServiceId_SetEvent         8
#define OSServiceId_ClearEvent       9
#define OSServiceId_GetEvent        10
#define OSServiceId_WaitEvent       11
#define OSServiceId_GetAlarmBase    12
#define OSServiceId_GetAlarm        13
#define OSServiceId_SetRelAlarm     14
#define OSServiceId_SetAbsAlarm     14
#define OSServiceId_CancelAlarm     16
#define OSServiceId_CounterTick     17
/* GetActiveApplicationMode, ShutdownOS never return an error */
#define OSServiceId_StartOS         18
#define OSServiceId_ForceSchedule   19

#ifdef __OO_SEM__
/* InitSem, TryWaitSem, GetValueSem never return an error */
#define OSServiceId_WaitSem         20
#define OSServiceId_PostSem         20
#endif



/* 13.8.4 Macros                                                           */
/* ----------------------------------------------------------------------- */

union EE_oo_ErrorHook_parameters {
  struct {
    TaskType TaskID;
  } ActivateTask_prm;

#ifndef __OO_NO_CHAINTASK__
  struct {
    TaskType TaskID;
  } ChainTask_prm;
#endif

  struct {
    TaskType TaskID;
    TaskStateRefType State;
  } GetTaskState_prm;

#ifndef __OO_NO_RESOURCES__
  struct {
    ResourceType ResID;
  } GetResource_prm;

  struct {
    ResourceType ResID;
  } ReleaseResource_prm;
#endif


  struct {
    TaskType TaskID;
    EventMaskType Mask;
  } SetEvent_prm;

  struct {
    EventMaskType Mask;
  } ClearEvent_prm;

  struct {
    TaskType TaskID;
    EventMaskRefType Event;
  } GetEvent_prm;

  struct {
    EventMaskType Mask;
  } WaitEvent_prm;


#ifndef __OO_NO_ALARMS__
  struct {
    AlarmType AlarmID;
    AlarmBaseRefType Info;
  } GetAlarmBase_prm;

  struct {
    AlarmType AlarmID;
    TickRefType Tick;
  } GetAlarm_prm;

  struct {
    AlarmType AlarmID;
    TickType increment;
    TickType cycle;
  } SetRelAlarm_prm;

  struct {
    AlarmType AlarmID;
    TickType start;
    TickType cycle;
  } SetAbsAlarm_prm;

  struct {
    AlarmType AlarmID;
  } CancelAlarm_prm;

  struct {
    AlarmType AlarmID;
    TaskType TaskID;

    EventMaskType Mask;

    EE_TYPENOTIFY action;
  } CounterTick_prm;

#endif

  struct {
    AppModeType Mode;
  } StartOS_prm;

#ifdef __OO_SEM__
  struct {
    SemRefType Sem;
  } WaitSem_prm;

  struct {
    SemRefType Sem;
  } PostSem_prm;
#endif
};

extern OSServiceIdType EE_oo_ErrorHook_ServiceID;
extern union EE_oo_ErrorHook_parameters EE_oo_ErrorHook_data;

#endif /* __OO_ERRORHOOK_NOMACROS__ */
#endif /* __OO_HAS_ERRORHOOK__ */

#endif
