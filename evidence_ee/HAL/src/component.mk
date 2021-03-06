# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
# 
# Copyright (C) 2002-2008  Evidence Srl
# 
# This file is part of ERIKA Enterprise.
# 
# ERIKA Enterprise is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation, 
# (with a special exception described below).
# 
# Linking this code statically or dynamically with other modules is
# making a combined work based on this code.  Thus, the terms and
# conditions of the GNU General Public License cover the whole
# combination.
# 
# As a special exception, the copyright holders of this library give you
# permission to link this code with independent modules to produce an
# executable, regardless of the license terms of these independent
# modules, and to copy and distribute the resulting executable under
# terms of your choice, provided that you also meet, for each linked
# independent module, the terms and conditions of the license of that
# module.  An independent module is a module which is not derived from
# or based on this library.  If you modify this code, you may extend
# this exception to your version of the code, but you are not
# obligated to do so.  If you do not wish to do so, delete this
# exception statement from your version.
# 
# ERIKA Enterprise is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License version 2 for more details.
# 
# You should have received a copy of the GNU General Public License
# version 2 along with ERIKA Enterprise; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301 USA.
# ###*E*###

#
# Evidence Srl ERIKA Enterprise component.mk
#

# This file describes the files needed for the Altera Nios II port of
# ERIKA Enterprise. It is present only for source distributions.

# Note: for a source distribution, the EE code is compiled together
# with the application. For that reason, the source code is listed
# using C_SRCS and not C_LIB_SRCS


CFLAGS += -I$(SOPC_KIT_NIOS2)/components/evidence_ee/ee/pkg
VPATH += $(SOPC_KIT_NIOS2)/components/evidence_ee/


ifeq ($(findstring __OO_BCC1__,$(EEOPT)) , __OO_BCC1__)
EE_OO=YES
endif
ifeq ($(findstring __OO_BCC2__,$(EEOPT)) , __OO_BCC2__)
EE_OO=YES
endif
ifeq ($(findstring __OO_ECC1__,$(EEOPT)) , __OO_ECC1__)
EE_OO=YES
EE_OO_ECC=YES
endif
ifeq ($(findstring __OO_ECC2__,$(EEOPT)) , __OO_ECC2__)
EE_OO=YES
EE_OO_ECC=YES
endif


# CPU: __NIOS2__
# ------------------------------------------------------------------------------------

ifeq ($(findstring __NIOS2__,$(EEOPT)) , __NIOS2__)

EE_C_SRCS  += \
	ee/pkg/cpu/nios2/src/ee_start.c \
	ee/pkg/mcu/altera_nios2/src/ee_nios2_timers.c

EE_ASM_SRCS += \
	ee/pkg/cpu/nios2/src/ee_hal.S

ifeq ($(EE_OO), YES)
EE_ASM_SRCS += \
	ee/pkg/cpu/nios2/src/ee_oo.S
endif

endif


# Kernel: __OO_*__
# ------------------------------------------------------------------------------------


ifeq ($(EE_OO), YES)
EE_C_SRCS += \
	ee/pkg/kernel/oo/src/ee_activate.c \
	ee/pkg/kernel/oo/src/ee_terminat.c \
	ee/pkg/kernel/oo/src/ee_thendin.c \
	ee/pkg/kernel/oo/src/ee_irqendin.c \
	ee/pkg/kernel/oo/src/ee_tstub.c \
	ee/pkg/kernel/oo/src/ee_lookup.c \
	ee/pkg/kernel/oo/src/ee_rq_exchg.c \
	ee/pkg/kernel/oo/src/ee_rq_first.c \
	ee/pkg/kernel/oo/src/ee_rq_inser.c \
	ee/pkg/kernel/oo/src/ee_startos.c

ifneq ($(findstring __OO_NO_RESOURCES__,$(EEOPT)) , __OO_NO_RESOURCES__)
EE_C_SRCS += \
	ee/pkg/kernel/oo/src/ee_lockres.c \
	ee/pkg/kernel/oo/src/ee_ulockres.c
endif

ifeq ($(EE_OO_ECC) , YES)
EE_C_SRCS += \
	ee/pkg/kernel/oo/src/ee_evclear.c \
	ee/pkg/kernel/oo/src/ee_evget.c \
	ee/pkg/kernel/oo/src/ee_evset.c \
	ee/pkg/kernel/oo/src/ee_evwait.c 
endif

ifeq ($(findstring __BIN_DISTR,$(EEOPT)) , __BIN_DISTR)
EE_C_SRCS += \
	ee/pkg/kernel/oo/src/ee_disableallinterrupts.c     \
	ee/pkg/kernel/oo/src/ee_enableallinterrupts.c      \
	ee/pkg/kernel/oo/src/ee_suspendallinterrupts.c     \
	ee/pkg/kernel/oo/src/ee_resumeallinterrupts.c      \
	ee/pkg/kernel/oo/src/ee_suspendosinterrupts.c      \
	ee/pkg/kernel/oo/src/ee_resumeosinterrupts.c       \
	ee/pkg/kernel/oo/src/ee_getactiveapplicationmode.c \
	ee/pkg/kernel/oo/src/ee_gettaskid.c                \
	ee/pkg/kernel/oo/src/ee_gettaskstate.c
endif

ifeq ($(findstring __OO_SEM__,$(EEOPT)) , __OO_SEM__)
EE_C_SRCS += \
	ee/pkg/kernel/oo/src/ee_sempost.c                  \
	ee/pkg/kernel/oo/src/ee_semtrywait.c

ifeq ($(EE_OO_ECC) , YES)
EE_C_SRCS += \
	ee/pkg/kernel/oo/src/ee_semwait.c
endif
endif
endif

# Kernel: __FP__
# ------------------------------------------------------------------------------------

ifeq ($(findstring __FP__,$(EEOPT)) , __FP__)

EE_C_SRCS  += \
	ee/pkg/kernel/fp/src/ee_irqsc.c \
	ee/pkg/kernel/fp/src/ee_rqexchg.c \
	ee/pkg/kernel/fp/src/ee_rqinsert.c \
	ee/pkg/kernel/fp/src/ee_schedule.c \
	ee/pkg/kernel/fp/src/ee_thact.c \
	ee/pkg/kernel/fp/src/ee_thendin.c \

  ifneq ($(findstring __FP_NO_MUTEX__,$(EEOPT)) , __FP_NO_MUTEX__)
  EE_C_SRCS  += ee/pkg/kernel/fp/src/ee_mutex.c
  endif

endif

# Kernel: __RN__
# ------------------------------------------------------------------------------------

ifeq ($(findstring __RN__,$(EEOPT)) , __RN__)
EE_C_SRCS  += \
	ee/pkg/kernel/rn/src/ee_rn.c \
	ee/pkg/kernel/rn/src/ee_rnsend.c
endif

# Kernel: __ALARMS__
# ------------------------------------------------------------------------------------


# COM: __CABS__
# ------------------------------------------------------------------------------------

ifeq ($(findstring __CABS__,$(EEOPT)) , __CABS__)
EE_C_SRCS  += \
	ee/pkg/com/cabs/src/ee_cabs.c
endif

# Test: __ASSERT__
# ------------------------------------------------------------------------------------

ifeq ($(findstring __ASSERT__,$(EEOPT)) , __ASSERT__)
EE_C_SRCS  += \
	ee/pkg/test/assert/src/ee_assert.c
endif



# Kernel: __FRSH__
# ------------------------------------------------------------------------------------

ifeq ($(findstring __FRSH__,$(EEOPT)) , __FRSH__)
CFLAGS += -I$(SOPC_KIT_NIOS2)/components/evidence_ee/ee/pkg/kernel/frsh/frsh_include


EE_C_SRCS += \
	ee/pkg/kernel/frsh/src/ee_cap.c \
	ee/pkg/kernel/frsh/src/ee_dlcheck.c \
	ee/pkg/kernel/frsh/src/ee_end_budget.c \
	ee/pkg/kernel/frsh/src/ee_end_recharging.c \
	ee/pkg/kernel/frsh/src/ee_frsh_bind.c \
	ee/pkg/kernel/frsh/src/ee_frsh_unbind.c \
	ee/pkg/kernel/frsh/src/ee_frsh_bind_utils.c \
	ee/pkg/kernel/frsh/src/ee_frsh_contracts.c \
	ee/pkg/kernel/frsh/src/ee_frsh_getvresid.c \
	ee/pkg/kernel/frsh/src/ee_frsh_getcontract.c \
	ee/pkg/kernel/frsh/src/ee_frsh_init.c \
	ee/pkg/kernel/frsh/src/ee_frsh_strerror.c \
	ee/pkg/kernel/frsh/src/ee_gettime.c \
	ee/pkg/kernel/frsh/src/ee_irq_sc.c \
	ee/pkg/kernel/frsh/src/ee_mutex.c \
	ee/pkg/kernel/frsh/src/ee_rcg_inser.c \
	ee/pkg/kernel/frsh/src/ee_rq_extract.c \
	ee/pkg/kernel/frsh/src/ee_rq_inser.c \
	ee/pkg/kernel/frsh/src/ee_schedule.c \
	ee/pkg/kernel/frsh/src/ee_thact.c \
	ee/pkg/kernel/frsh/src/ee_thendin.c

ifeq ($(findstring __FRSH_SINGLEIRQ__,$(EEOPT)) , __FRSH_SINGLEIRQ__)
EE_C_SRCS += \
	ee/pkg/kernel/frsh/src/ee_frsh_timers.c
endif

ifeq ($(findstring __FRSH_SYNCHOBJ__,$(EEOPT)) , __FRSH_SYNCHOBJ__)
EE_C_SRCS += \
	ee/pkg/kernel/frsh/src/ee_frsh_syncobj_signal.c \
	ee/pkg/kernel/frsh/src/ee_frsh_syncobj_wait.c \
	ee/pkg/kernel/frsh/src/ee_frsh_syncobj_waittimeout.c \
	ee/pkg/kernel/frsh/src/ee_frsh_syncobj_wait_utils.c \
	ee/pkg/kernel/frsh/src/ee_frsh_syncobj_timedwait.c

endif




endif







ifneq ($(findstring __BIN_DISTR_FULL__,$(EEOPT)) ,  __BIN_DISTR_FULL__)
ifneq ($(findstring __BIN_DISTR_LIMITED__,$(EEOPT)) ,  __BIN_DISTR_LIMITED__)
# This is not a binary distribution, we have to make a library
C_LIBSRCS += $(EE_C_SRCS)
ASM_LIBSRCS += $(EE_ASM_SRCS)
else
# Limited distribution
C_SRCS += $(EE_C_SRCS)
ASM_SRCS += $(EE_ASM_SRCS)
endif
else
# Full distribution
C_SRCS += $(EE_C_SRCS)
ASM_SRCS += $(EE_ASM_SRCS)
endif


