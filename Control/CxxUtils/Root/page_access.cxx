/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/page_access.h"
namespace athena{

  const void* page_address(const void* addr) { 
    return (const void*)((long)addr & ~(PAGESIZE-1)); 
  }
  void* page_address(void* addr) { 
    return (void*)((long)addr & ~(PAGESIZE-1)); 
  }
  void* next_page_address(void* addr) { 
    return (void*)(((long)addr & ~(PAGESIZE-1)) + PAGESIZE); 
  }

  int page_protect(void* addr, int prot) {
    int rc=mprotect(page_address(addr), PAGESIZE, prot);
    if (rc) printf("page_protect WARNING: mprotect heap failed for void *address %p\n", addr);
#ifdef DEBUG
    else printf("page_protect DEBUG: set protection @%i for range @%lx - @%lx containing void* addr=%p\n",
		prot,(long unsigned int)page_address(addr), 
		(long unsigned int)page_address(addr) + PAGESIZE, addr);
#endif
    return rc;
  }
}
