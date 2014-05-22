/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <gperftools/malloc_hook_c.h>


size_t MallocExtension_GetAllocatedSize(void* p){
	return 0;
}

MallocHook_NewHook MallocHook_SetNewHook(MallocHook_NewHook hook) {
	//printf("dummy");
  return 0;
}

MallocHook_DeleteHook MallocHook_SetDeleteHook(MallocHook_DeleteHook hook) {
	//printf("dummy");
	return 0;
}

void MallocExtension_GetStats(char* buffer, int buffer_length){
 return ;
}


