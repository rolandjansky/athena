/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <boost/pool/pool_alloc.hpp>
#include "CxxUtils/procmaps.h"
//#define DEBUGIT 1
using namespace std;
void nop() {}
int main(void) {
  cout << "*** procmaps_test starts ***" <<endl;
#ifdef DEBUGIT
  const bool DUMPMAPS(true);
#else
  const bool DUMPMAPS(false);
#endif
  procmaps pmaps(DUMPMAPS);
  //let's start with a code page
  const procmaps::Entry* pCodeEntry(pmaps.getEntry((void *)(unsigned long)&nop));
  assert(pCodeEntry); 
#ifdef DEBUGIT
  cout << "code entry " << hex << pCodeEntry->begAddress << " " 
       << (void *)&nop  << " "
       << pCodeEntry->endAddress << endl;
#endif
  assert(pCodeEntry->executable);
  assert(pCodeEntry->readable);
  assert(!pCodeEntry->writable);
  assert(0 != pCodeEntry->inode);
  //now with a heap page
  int* pi= new int(2);
  const procmaps::Entry* pHeapEntry(pmaps.getEntry(pi));
  assert(pHeapEntry); 
#ifdef DEBUGIT
  cout << "heap entry " << hex << pHeapEntry->begAddress << " " 
       << pi  << " "
       << pHeapEntry->endAddress << endl;
#endif
  //FIXME the heap should not be executable, right?  assert(!pHeapEntry->executable);
  assert(pHeapEntry->readable);
  assert(pHeapEntry->writable);
  assert(0 == pHeapEntry->inode);

//make valgrind happy
  delete pi;
  boost::singleton_pool<boost::pool_allocator_tag, sizeof(procmaps::Entry)>::release_memory();

  cout << "*** procmaps_test OK ***" <<endl;
  return 0;
}
