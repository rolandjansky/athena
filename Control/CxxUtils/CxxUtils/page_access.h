// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CXXUTILS_PAGEADDRESS_H
#define CXXUTILS_PAGEADDRESS_H 1
#include <cstdio>
#include <sys/mman.h> /* mprotect */
#define DEBUG 1
namespace athena {
  const size_t PAGESIZE= 0x1000;  //sysconf(_SC_PAGE_SIZE)

  const void* page_address(const void* addr);
  void* page_address(void* addr);
  void* next_page_address(void* addr);
  int page_protect(void* addr, int prot);
/// protect page containing addr, from page boundary to addr+sizeof(T). 
/// @returns amount of mem protected  (0 flags an error)
template <typename T>
size_t page_protect(T* addr, int prot) {
  void* pageAddr(page_address((void*)addr));
  size_t lProtected((long)addr-(long)pageAddr+sizeof(T));
  int rc=mprotect(pageAddr, lProtected, prot);
  if (rc) printf("page_protect WARNING: mprotect heap failed for address %p\n", (void*)addr);
#ifdef DEBUG
  else printf("page_protect DEBUG: set protection @%i for range @%lx - @%zx containing addr=%p\n",
	      prot,(long unsigned int)page_address(addr),(long unsigned int)addr+sizeof(T), (void*)addr);
#endif
  return (rc == 0 ? lProtected : 0);
}
}
#endif
