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
 * CVS: $Id: ee_chaintas.c,v 1.3 2005/07/19 14:01:16 pj Exp $
 */

#include "ee_internal.h"

/* ChainTask: 
  - activate a task 
    - The task is moved from the running state to the suspended state 
      and the other is activated (it can be the same task)
    - automatic release of internal resource 
    - Rescheduling is issued 
    - returns 
      E_OS_LIMIT if too many activations
      Extended status
      E_OS_ID if the task id is invalid
      E_OS_RESOURCE if the task still occupy resources
      E_OS_CALLLEVEL if called at interrupt level 
*/

#ifndef __OO_NO_CHAINTASK__
#ifndef __PRIVATE_CHAINTASK__
StatusType EE_oo_ChainTask(TaskType TaskID)
{
  register TaskType current;

#ifdef __OO_ORTI_SERVICETRACE__
  EE_ORTI_servicetrace = EE_SERVICETRACE_CHAINTASK+1;
#endif

  current = EE_stk_queryfirst();

#ifdef __OO_EXTENDED_STATUS__

  /* check for a call at interrupt level: This must be the FIRST check! */
  if (EE_hal_get_IRQ_nesting_level()) {
#ifdef __OO_ORTI_LASTERROR__
    EE_ORTI_lasterror = E_OS_CALLEVEL;
#endif

#ifdef __OO_HAS_ERRORHOOK__
    EE_hal_begin_primitive();
    if (!EE_ErrorHook_nested_flag) {
#ifndef __OO_ERRORHOOK_NOMACROS__
      EE_oo_ErrorHook_ServiceID = OSServiceId_ChainTask;
      EE_oo_ErrorHook_data.ChainTask_prm.TaskID = TaskID;
#endif
      EE_ErrorHook_nested_flag = 1;
      ErrorHook(E_OS_CALLEVEL);
      EE_ErrorHook_nested_flag = 0;
    }
    EE_hal_end_primitive();
#endif

#ifdef __OO_ORTI_SERVICETRACE__
    EE_ORTI_servicetrace = EE_SERVICETRACE_CHAINTASK;
#endif

    return E_OS_CALLEVEL;
  }

  /* check if the task Id is valid */
  if (TaskID < 0 || TaskID >= EE_MAX_TASK) {
#ifdef __OO_ORTI_LASTERROR__
    EE_ORTI_lasterror = E_OS_ID;
#endif

#ifdef __OO_HAS_ERRORHOOK__
    EE_hal_begin_primitive();
    if (!EE_ErrorHook_nested_flag) {
#ifndef __OO_ERRORHOOK_NOMACROS__
      EE_oo_ErrorHook_ServiceID = OSServiceId_ChainTask;
      EE_oo_ErrorHook_data.ChainTask_prm.TaskID = TaskID;
#endif
      EE_ErrorHook_nested_flag = 1;
      ErrorHook(E_OS_ID);
      EE_ErrorHook_nested_flag = 0;
    }
    EE_hal_end_primitive();
#endif

#ifdef __OO_ORTI_SERVICETRACE__
    EE_ORTI_servicetrace = EE_SERVICETRACE_CHAINTASK;
#endif
    return E_OS_ID;
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
      EE_oo_ErrorHook_ServiceID = OSServiceId_ChainTask;
      EE_oo_ErrorHook_data.ChainTask_prm.TaskID = TaskID;
#endif
      EE_ErrorHook_nested_flag = 1;
      ErrorHook(E_OS_RESOURCE);
      EE_ErrorHook_nested_flag = 0;
    }
    EE_hal_end_primitive();
#endif

#ifdef __OO_ORTI_SERVICETRACE__
    EE_ORTI_servicetrace = EE_SERVICETRACE_CHAINTASK;
#endif

    return E_OS_RESOURCE;
  }
#endif

#endif




  EE_hal_begin_primitive();

  /* check for pending activations; works also if the task passed as
     parameter inside ChainTask is the calling task; 
     see MODISTARC Test 9 */
  if (TaskID != current &&
      EE_th_rnact[TaskID] == 0) {
#ifdef __OO_ORTI_LASTERROR__
    EE_ORTI_lasterror = E_OS_LIMIT;
#endif

#ifdef __OO_HAS_ERRORHOOK__
    if (!EE_ErrorHook_nested_flag) {  
#ifndef __OO_ERRORHOOK_NOMACROS__
      EE_oo_ErrorHook_ServiceID = OSServiceId_ChainTask;
      EE_oo_ErrorHook_data.ChainTask_prm.TaskID = TaskID;
#endif
      EE_ErrorHook_nested_flag = 1;
      ErrorHook(E_OS_LIMIT);
      EE_ErrorHook_nested_flag = 0;
    }
#endif

    EE_hal_end_primitive();

#ifdef __OO_ORTI_SERVICETRACE__
    EE_ORTI_servicetrace = EE_SERVICETRACE_CHAINTASK;
#endif

    return E_OS_LIMIT;
  } 

#ifdef __RN_TASK__
  if (TaskID & EE_REMOTE_TID) 
    {
      EE_TYPERN_PARAM par;
      par.pending = 1;
      /* forward the request to another CPU */
      EE_rn_send(TaskID & ~EE_REMOTE_TID, EE_RN_TASK, par );
      /* like in TerminateTask, we do not have to activate any other task */
      EE_th_terminate_nextask[EE_stk_queryfirst()] = EE_NIL;
    }
  else
#endif
    {
      /* Note: the decrement is done now, not in the endcycle! */
      EE_th_rnact[TaskID]--;  
      /* and we have to activate another task... */ 
      EE_th_terminate_nextask[current] = TaskID;
    }
  
#ifdef __OO_ORTI_SERVICETRACE__
  EE_ORTI_servicetrace = EE_SERVICETRACE_CHAINTASK;
#endif

  EE_hal_terminate_task(current);

  /* This return instruction usually is optimized by the compiler,
     because hal_terminate_task does not return... */
  return E_OK;
}

#endif
#endif
