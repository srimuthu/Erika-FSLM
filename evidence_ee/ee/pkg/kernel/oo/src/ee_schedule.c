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
 * Author: 2002 Paolo Gai
 * CVS: $Id: ee_schedule.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

#include "ee_internal.h"

/* Schedule: 
    - the internal resource is released; 
    - no check if resources are still used by the task
    - then rescheduling takes place 
    - and then the internal resource is taken again
    - returns
      Extended status
      E_OS_CALLLEVEL if called at interrupt level 
      E_OS_RESOURCE if the calling task occupies resources
*/

#ifndef __PRIVATE_SCHEDULE__

#ifdef __OO_EXTENDED_STATUS__
StatusType EE_oo_Schedule(void)
#else
void EE_oo_Schedule(void)
#endif
{
  EE_TID current, rq;

#ifdef __OO_ORTI_SERVICETRACE__
  EE_ORTI_servicetrace = EE_SERVICETRACE_SCHEDULE+1;
#endif

  current = EE_stk_queryfirst();

#ifdef __OO_EXTENDED_STATUS__
  /* check for a call at interrupt level: This must be the FIRST check!*/
  if (EE_hal_get_IRQ_nesting_level()) {
#ifdef __OO_ORTI_LASTERROR__
    EE_ORTI_lasterror = E_OS_CALLEVEL;
#endif

#ifdef __OO_HAS_ERRORHOOK__
    EE_hal_begin_primitive();
    if (!EE_ErrorHook_nested_flag) {  
#ifndef __OO_ERRORHOOK_NOMACROS__
      EE_oo_ErrorHook_ServiceID = OSServiceId_Schedule;
#endif
      EE_ErrorHook_nested_flag = 1;
      ErrorHook(E_OS_CALLEVEL);
      EE_ErrorHook_nested_flag = 0;
    }
    EE_hal_end_primitive();
#endif

#ifdef __OO_ORTI_SERVICETRACE__
    EE_ORTI_servicetrace = EE_SERVICETRACE_SCHEDULE;
#endif

    return E_OS_CALLEVEL;
  }


#ifndef __OO_NO_RESOURCES__
  /* check for busy resources */
  if (EE_th_resource_last[current] != -1) {
#ifdef __OO_ORTI_LASTERROR__
    EE_ORTI_lasterror = E_OS_RESOURCE;
#endif

#ifdef __OO_HAS_ERRORHOOK__
    EE_hal_begin_primitive();
    if (!EE_ErrorHook_nested_flag) {  
#ifndef __OO_ERRORHOOK_NOMACROS__
      EE_oo_ErrorHook_ServiceID = OSServiceId_Schedule;
#endif
      EE_ErrorHook_nested_flag = 1;
      ErrorHook(E_OS_RESOURCE);
      EE_ErrorHook_nested_flag = 0;
    }
    EE_hal_end_primitive();
#endif

#ifdef __OO_ORTI_SERVICETRACE__
    EE_ORTI_servicetrace = EE_SERVICETRACE_SCHEDULE;
#endif

    return E_OS_RESOURCE;
  }
#endif

#endif




  EE_hal_begin_primitive();
  
  /* check if there is a preemption */
  rq = EE_rq_queryfirst();
  if (rq != EE_NIL) {
    /* The standard says that "Schedule enables a processor assignment
       to other tasks with lower priority than the ceiling priority of
       the internal resource and higher priority than the priority of
       the calling task". That means that only tasks currently in the
       ready queue with the ready priority > than the ready priority
       of the running task can be executed... */
    if (EE_th_ready_prio[current] < EE_th_ready_prio[rq]) {
#ifdef __OO_HAS_POSTTASKHOOK__
      PostTaskHook();
#endif	
      /* release the internal resource */
      EE_sys_ceiling &= ~EE_th_dispatch_prio[current];

      /* set the ready priority bit. In that way we prevent preemption
       * from all the tasks with lower priority than the current task.
       *
       * NOTE: Setting the ready priority is legal because if the task
       * has been scheduled it must be that the system_ceiling <
       * ready_priority, and so (system_ceiling &ready_priority)=0!!!
       * after a task has been put in execution, the dispatch priority
       * is set. no other bits are set when this function is called
       * (all the resources must be unlocked, and all the task that
       * preempts the running task must have been finished!.
       */
      EE_sys_ceiling |= EE_th_ready_prio[current];

      /* we have to put the task in the ready status */
      EE_th_status[current] = READY;
      /* but not in the ready queue!!! 
	 the task remains into the stacked queue!
      */
    
      /* get the new internal resource */
      EE_sys_ceiling |= EE_th_dispatch_prio[rq];
      /* put the task in running state */
      EE_th_status[rq] = RUNNING;

#ifdef __OO_ORTI_PRIORITY__
      EE_ORTI_th_priority[current] = EE_th_ready_prio[current];
      EE_ORTI_th_priority[rq] = EE_th_dispatch_prio[rq];
#endif
      
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
      rq = EE_rq2stk_exchange();
      if (EE_th_waswaiting[rq]) {
	EE_th_waswaiting[rq] = 0;
	EE_hal_stkchange(rq); 
      }
      else
	EE_hal_ready2stacked(rq);
#else
      EE_hal_ready2stacked(EE_rq2stk_exchange());
#endif
      
      /* release the ready priority bit and... */
      EE_sys_ceiling &= ~EE_th_ready_prio[current];
      /* ...get again the internal resource */
      EE_sys_ceiling |= EE_th_dispatch_prio[current];

#ifdef __OO_ORTI_PRIORITY__
      EE_ORTI_th_priority[current] = EE_th_dispatch_prio[current];
#endif

#ifdef __OO_HAS_PRETASKHOOK__
      PreTaskHook();
#endif	

    }
  }
  
  EE_hal_end_primitive();

#ifdef __OO_ORTI_SERVICETRACE__
  EE_ORTI_servicetrace = EE_SERVICETRACE_SCHEDULE;
#endif
  
#ifdef __OO_EXTENDED_STATUS__
  return E_OK;
#endif
}

#endif

