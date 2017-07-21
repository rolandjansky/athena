/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define DEBUG 1
#include <algorithm> /* sort etc */
#include <sys/mman.h> /* mprotect */
#include "CxxUtils/PageAccessControl.h"
#include "CxxUtils/page_access.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
using athena::page_address;
using athena::next_page_address;

PageAccessControl::Entry::Entry(const void* a, size_t l, int p, void* pl): 
  addr(page_address(a)), lenProt(l), prot(p), leak(pl), restored(0) {}


void
PageAccessControl::sort() {
  if (!m_protectedIsSorted) {
    std::sort(m_protected.begin(), m_protected.end());
    m_protectedIsSorted=true;
  }
}
bool
PageAccessControl::restorePageProt(const void * addr) {
  int rc(-1);
  sort();
  Entry ea(addr,0,0,0);
  protected_t::iterator entry=
    std::lower_bound(m_protected.begin(), m_protected.end(), ea);
  if (entry != m_protected.end() &&
      entry->addr == ea.addr) {
    //found it. Restore page prot
    const void* pageAddr = page_address(entry->addr);
    void* pageAddr_nc ATLAS_THREAD_SAFE = const_cast<void*> (pageAddr);
    rc=mprotect( pageAddr_nc, entry->lenProt, entry->prot);
    if (rc==0) {
#ifdef DEBUG
      printf("PageAccessControl::restorePageProt DEBUG: restored protection %i for page %p containing address %p \n",
	     entry->prot,
	     page_address(entry->addr), 
	     entry->addr);
      printf(" FIXME NOT Freeing memory at %p \n", entry->leak );
#endif
      //      free(entry->leak);
      entry->leak=0;
      ++(entry->restored);
    }
  } else printf("WARNING no entry in procmap for addr=%p, page protection not restored \n",addr);
  return (rc == 0);
}

bool
PageAccessControl::protectPage(const void* addr, size_t objSize, int prot) {
  int  rc(-1);
  const procmaps::Entry *e=m_pmaps.getEntry(addr,true);
  //this is the length of the range we are going to protect
  if (0 != e) {
    const void *pageAddr = page_address(addr);
    size_t lenProt = (size_t)addr - (size_t)(pageAddr) + objSize;
    size_t nextProt = (size_t)addr + objSize;
    size_t nextUnprot = (size_t)(next_page_address((void*)(nextProt-1)));
    int pageProt(PROT_NONE);
    if (e->readable) pageProt |= PROT_READ;
    if (e->writable) pageProt |= PROT_WRITE;
    if (e->executable) pageProt |= PROT_EXEC;
    if (pageProt != prot) {
      //fill up the space from nextProt to nextUnprot to avoid allocations
      //in the locked pages, and SEGVs...
//       void *leak(0);
//       if (0 == (prot & PROT_WRITE)) {
// 	size_t lenLeak(nextUnprot-nextProt-1);
// 	leak=malloc(lenLeak);
// 	if ((size_t)leak<nextUnprot && (size_t)leak + lenLeak>=nextUnprot) {
// 	  //we do not want to allocate our buffer memory past the current
// 	  //page, so trim it down
// 	  free(leak);
// 	  lenLeak=nextUnprot - (size_t)leak -1;
// 	  leak=malloc(lenLeak);
// 	} 
// 	if (leak < pageAddr ||
// 	    (size_t)leak >= nextUnprot) {
// 	  //leak has been allocated into previous/next page
// 	  //better get rid of it as it will likely
// 	  //be locked by another protectPage
// 	  free(leak);
// 	  leak=0; 
// 	} else {
// #ifdef DEBUG
// 	  printf("PageAccessControl::protectPage DEBUG: fill up space from %p to 0x%x to avoid allocations in locked pages\n",
// 		 leak, (int)leak+lenLeak);
// #endif
// 	}
//       }

      void* pageAddr_nc ATLAS_THREAD_SAFE = const_cast<void*> (pageAddr);
      if (0 == (rc = mprotect( pageAddr_nc,
			       lenProt,
			       prot))) {
	m_protected.push_back(Entry(pageAddr,lenProt, pageProt, 0));
	m_protectedIsSorted=false; //FIXME we should use a mapvector
#ifdef DEBUG
	printf("PageAccessControl::protectPage DEBUG: set protection %i for page range %p - 0x%lx containing address range=%p - 0x%lx\n",
	       prot,
	       pageAddr, 
	       (long unsigned int)(nextUnprot - 1), 
	       addr,
	       (long unsigned int)(nextProt -1) );
#endif
      }
    } else rc=0;
  } else printf("PageAccessControl::protectPage WARNING: no entry in procmap for addr=%p, page protection not restored \n",addr);
  return (rc == 0);
}

bool PageAccessControl::accessed(const void* address) const {
  bool acc(false);
  //fixme: poor man implementation
  Entry eaxx(address,0,0,0);
  PageAccessControl::const_iterator ia(beginProtectedPtrs()),
    ea(endProtectedPtrs());
  while (!acc && ia != ea) {
    //    std::cout << address << "page addr " << eaxx.addr << " ia " << ia->addr << " res " << ia->restored << std::endl;
    acc = (eaxx.addr == ia->addr && 0 != ia->restored);
    ++ia;
  }
  return acc;
}
