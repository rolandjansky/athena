/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <boost/pool/pool_alloc.hpp>
#include "CxxUtils/procmaps.h"
#include "CxxUtils/PageAccessControl.h"
// #define DEBUGIT 1
using namespace std;
int main(void) {
  cout << "*** PageAccessControl_test starts ***" <<endl;
#ifdef DEBUGIT
  const bool DUMPMAPS(true);
#else
  const bool DUMPMAPS(false);
#endif
  procmaps pmaps(DUMPMAPS);
  PageAccessControl pac(pmaps);
  //protect a heap object
  int* pi= new int(2);
  void* pv = malloc(10);
  assert(pac.forbidPage(pi));
  //FIXME assert(pac.forbidPage(pv, 10));
  //assert(pac.protectPage(pv, 10, PROT_READ));
  assert(pac.restorePageProt(pi));
  //assert(pac.restorePageProt(pv));
  cout << "accessing restored pointer " << *pi << endl;

  

//make valgrind happy
  delete pi;
  free(pv);

  cout << "*** PageAccessControl_test OK ***" <<endl;
  return 0;
}
