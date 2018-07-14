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
 * CVS: $Id: ee_thendin.c,v 1.3 2006/01/24 10:20:20 pj Exp $
 */

#include "ee_internal.h"

#ifndef __PRIVATE_THREAD_END_INSTANCE__
void EE_thread_end_instance(void)
{
    EE_TID current, rqfirst;
#ifndef __OO_NO_CHAINTASK__
    EE_TID TaskID;
#endif

    current = EE_stk_queryfirst();

#ifdef __OO_HAS_POSTTASKHOOK__
    PostTaskHook();
#endif	

    /* increase the remaining activations...*/
    EE_th_rnact[current]++;

    /* The task state switch from STACKED TO READY because it end its
     * instance. Note that status=READY and nact=0 ==>> the task is
     * suspended!!! */
    EE_th_status[current] = SUSPENDED;

    /* reset the thread priority bit in the system_ceiling */
    EE_sys_ceiling &= ~EE_th_dispatch_prio[current];

#ifdef __OO_ORTI_PRIORITY__
    EE_ORTI_th_priority[current] = 0;
#endif

    /* extract the task from the stk data structure */
    EE_stk_getfirst();

#ifndef __OO_NO_CHAINTASK__
    /* if we called a ChainTask, 
       EE_th_terminate_nextask[current] != NIL
    */
    TaskID = EE_th_terminate_nextask[current];
    if (TaskID != EE_NIL) {
      /* see also activate.c
	 put the task in the ready state:
	 - if the task is basic/BCC1 or extended it can be here only because
	 it had rnact=1 before the call, and so it is in suspended state
	 - if the task is basic/BCC2 it can be that it is ready or 
	 running. in that case we have to check and queue it anyway
      */
#if defined(__OO_BCC2__) || defined(__OO_ECC2__)
      if (EE_th_status[TaskID] == SUSPENDED) {
	EE_th_status[TaskID] = READY;
#ifdef __OO_ECC2__
	/* When an extended task is transferred from suspended state
	   into ready state all its events are cleared*/
	EE_th_event_active[TaskID] = 0;
#endif
      }
#else
      EE_th_status[TaskID] = READY;
#ifdef __OO_ECC1__
      /* When an extended task is transferred from suspended state
	 into ready state all its events are cleared*/
      EE_th_event_active[TaskID] = 0;
#endif
#endif
      /* insert the task in the ready queue */
      EE_rq_insert(TaskID);
    }
#endif

    /* check if there is to schedule a ready thread or pop a preempted
     * thread */
    rqfirst = EE_rq_queryfirst();
    if (rqfirst == EE_NIL) {
      /* No threads in the ready queue, return to the preempted thread (maybe main) */
      if (EE_stk_queryfirst() != EE_NIL) {
	EE_th_status[EE_stk_queryfirst()] = RUNNING;
#ifdef __OO_HAS_PRETASKHOOK__
	PreTaskHook();
#endif	
      }
      EE_hal_endcycle_stacked(EE_stk_queryfirst());
    }
    else if (EE_sys_ceiling >= EE_th_ready_prio[rqfirst]) {
	/* we have to schedule an interrupted thread (already on the
	 * stack!!!) */
	EE_th_status[EE_stk_queryfirst()] = RUNNING;
#ifdef __OO_HAS_PRETASKHOOK__
	PreTaskHook();
#endif	
	EE_hal_endcycle_stacked(EE_stk_queryfirst());
    }
    else { 
	/* we have to schedule a ready thread */
	EE_th_status[rqfirst] = RUNNING;
	EE_sys_ceiling |= EE_th_dispatch_prio[rqfirst];

#ifdef __OO_ORTI_PRIORITY__
	EE_ORTI_th_priority[rqfirst] = EE_th_dispatch_prio[rqfirst];
#endif

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
	/* Note: I reused rqfirst! */
	rqfirst = EE_rq2stk_exchange();
	if (EE_th_waswaiting[rqfirst]) {
	  EE_th_waswaiting[rqfirst] = 0;
#ifdef __OO_HAS_PRETASKHOOK__
	  PreTaskHook();
#endif	
	  EE_hal_endcycle_stacked(rqfirst);
	}
	else
	  EE_hal_endcycle_ready(rqfirst);
#else
	EE_hal_endcycle_ready(EE_rq2stk_exchange());
#endif
    }

    /* Remember: after hal_endcycle_XXX there MUST be NOTHING!!! */
}  
#endif
