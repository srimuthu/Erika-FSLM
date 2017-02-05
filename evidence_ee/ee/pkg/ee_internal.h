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
 * Author: 2000-2006 Paolo Gai
 * CVS: $Id: ee_internal.h,v 1.24 2008/07/04 14:26:59 francesco Exp $
 */

#ifndef __EE_INTERNAL_H__
#define __EE_INTERNAL_H__

#include "eecfg.h"


#if defined(__cplusplus)
extern "C" {
#endif



#include "ee_opt.h"




  /*
   *
   * CPU
   *
   */


#ifdef __AVR5__
#include "cpu/avr5/inc/ee_internal.h"
#endif

#ifdef __ST10__
#include "st10mono/internal.h"
#endif

#ifdef __ST10SEGM__
#include "st10segm/internal.h"
#endif

#if defined(__ARM7GNU__) || defined(__ARM7ADS__)
#include "cpu/arm7tdmi/inc/ee_internal.h"
#endif

#ifdef __MPC5XX__
#include "cpu/mpc5xx/inc/ee_internal.h"
#endif

#ifdef __MPC5PROTECTED__
#include "cpu/mpc5protected/inc/ee_internal.h"
#endif

#ifdef __NIOS2__
#include "cpu/nios2/inc/ee_internal.h"
#endif

#ifdef __PIC30__
#include "cpu/pic30/inc/ee_internal.h"
#endif

#ifdef __TRICORE1__
#include "cpu/tricore1/inc/ee_internal.h"
#endif


  /*
   *
   * MCU
   *
   */

  /* Atmel */
#if defined __ATMEGA128__
#include "mcu/atmel_atmega128/inc/ee_internal.h"
#endif


#if defined __ATMEGA1281__
#include "mcu/atmel_atmega1281/inc/ee_internal.h"
#endif


  /* ARM */
#if defined(__SAMSUNG_KS32C50100__)
#include "mcu/samsung_ks32c50100/inc/ee_internal.h"
#endif

#if defined(__UNIBO_MPARM__)
#include "mcu/unibo_mparm/inc/ee_internal.h"
#endif

#if defined __TRISCENDA7S__
#include "mcu/triscend_a7s/internal.h"
#endif

#if defined(__ST_STA2051__)
#include "mcu/st_sta2051/inc/ee_internal.h"
#endif

  /* PPC */
#ifdef __MPC566EVB__
#include "mcu/motorola_mpc566/inc/ee_mcu.h"
#endif

  /* Microchip dsPIC */
#ifdef __MICROCHIP_DSPIC30__
#include "mcu/microchip_dspic/inc/ee_internal.h"
#endif

  /* NIOS 2 */
#ifdef __NIOS2__
#include "mcu/altera_nios2/inc/ee_internal.h"
#endif


  /*
   *
   * Board
   *
   */

  /* Atmel */
#ifdef __ATMEL_STK50X__
#include "board/atmel_stk500/inc/ee_internal.h"
#endif

#ifdef __XBOW_MIB5X0__
#include "board/xbow_mib5x0/inc/ee_board.h"
#endif

  /* ARM */
#ifdef __EVALUATOR7T__
#include "board/arm_evaluator7t/inc/ee_internal.h"
#endif

#if defined(__unibo_mparm__)
#include "board/unibo_mparm/inc/ee_internal.h"
#endif

  /* Microchip dsPIC */
#ifdef __MICROCHIP_DSPICDEM11PLUS__
#include "board/microchip_dspicdem11plus/inc/ee_internal.h"
#endif

#ifdef __MICROCHIP_EXPLORER16__
#include "board/microchip_explorer16/inc/ee_internal.h"
#endif

#ifdef __EE_FLEX__
#include "board/ee_flex/inc/ee_internal.h"
#endif

  /*
   *
   * Kernel
   *
   */


#ifdef __FP__
#include "kernel/fp/inc/ee_internal.h"
#endif

#ifdef __EDF__
#include "kernel/edf/inc/ee_internal.h"
#endif

#ifdef __CBS__
#include "kernel/cbs/inc/ee_internal.h"
#endif

#ifdef __FRSH__
#ifdef __SEM_FRSH__
#include "kernel/frsh/syncobj/inc/ee_sem.h"
#endif
#include "kernel/frsh/inc/ee_internal.h"
#endif


#include "kernel/oo/inc/ee_internal.h"


#ifdef __SEM__
#include "kernel/sem/inc/ee_sem.h"
#endif

#ifdef __ALARMS__
#include "kernel/alarms/inc/ee_alarms.h"
#endif

#ifdef __RN__
#include "kernel/rn/inc/ee_rn_internal.h"
#endif



#if defined(__cplusplus)
};
#endif

#endif
