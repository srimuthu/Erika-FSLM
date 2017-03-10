#include "ee_internal.h"


StatusType EE_oo_GetResource(ResourceType ResID){
	register EE_UREG isGlobal;
	register EE_FREG flag;
	
	isGlobal = ResID & EE_GLOBAL_MUTEX;
	ResID = ResID & ~EE_GLOBAL_MUTEX;
	if (ResID >= EE_MAX_RESOURCE) { return E_OS_ID; }
	if (EE_resource_locked[ResID] || EE_th_ready_prio[EE_stkfirst] > EE_resource_ceiling[ResID]) {return E_OS_ACCESS;}

	flag = EE_hal_begin_nested_primitive();
        EE_resource_stack[ResID] = EE_stkfirst;
		EE_resource_locked[ResID] = 1;
		EE_sys_ceiling |= 0x80;
		if (isGlobal){ EE_hal_spin_in(ResID);}
	EE_hal_end_nested_primitive(flag);
	return E_OK;
}