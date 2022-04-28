/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <fstream>
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
  //assert(0 != pCodeEntry->inode);
  if (0 == pCodeEntry->inode) {
    printf ("Test fail: null inode %0lx-%0lx %u %u %u %u %u %u %u %lu %s\n",
            pCodeEntry->begAddress,
            pCodeEntry->endAddress,
            pCodeEntry->readable,
            pCodeEntry->writable,
            pCodeEntry->executable,
            pCodeEntry->isPrivate,
            pCodeEntry->offset,
            pCodeEntry->dev[0],
            pCodeEntry->dev[1],
            static_cast<unsigned long>(pCodeEntry->inode),
            pCodeEntry->pathname.c_str());
    fflush (stdout);
    std::ifstream f("/proc/self/maps");
    const int LMAX=256;
    char line[LMAX];
    while ( f.getline(line,LMAX) ) {
      printf("%s\n",line);
    }
    std::abort();
  }
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
