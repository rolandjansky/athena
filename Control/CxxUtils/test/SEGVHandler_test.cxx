/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#undef NDEBUG
#include <iostream>
#include <signal.h>   /*sigaction*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <utility> /* pair */
#include <cassert>
#include "CxxUtils/procmaps.h"
#include "CxxUtils/PageAccessControl.h"
#include "CxxUtils/PtrAccessSEGVHandler.h"
#include "CxxUtils/cPtrAccessSEGVHandler.h"
#include "CxxUtils/checker_macros.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace std;

int main(void) {
  cout << "*** SEGVHandler_test starts ***" <<endl;
  int rc=0;
  struct sigaction sa, stdSEGV;
  (void)sigaction(SIGSEGV,NULL,&stdSEGV);
  sa = stdSEGV;
  //  sa.sa_sigaction=trapReads;
  procmaps p;
  PageAccessControl pac(p);
  PtrAccessSEGVHandler h(pac);
  setPtrAccessSEGVHandler(&h);
  sa.sa_sigaction= cPtrAccessSEGVHandler; 
  sa.sa_flags=SA_SIGINFO;
  rc=sigaction(SIGSEGV,&sa,NULL);
  printf("sigaction installing handler returned %d\n",rc);

  size_t pagesz = getpagesize();
  char* pool = new char[4*pagesz];

  //  int *pInt=(int*)0x8000;
  //  int *pInt=(int*)&cPtrAccessSEGVHandler; 
  int *pInt=new (pool + pagesz) int(11);
  printf("@pInt=%p\n",(void*)pInt);
  //  delete pInt;
  string *pString(new (pool + 2*pagesz) string("bar"));
  printf("@pString=%p\n",(void*)pString);

  pair<int,double>* pPair(new (pool + 3*pagesz) pair<int,double>(1,2.0));
  printf("@pPair=%p\n",(void*)pPair);
  //now protect the pages.
  assert(pac.forbidPage(pInt));
  assert(pac.forbidPage(pString));
  assert(pac.forbidPage(pPair));
  //  p.loadMaps(true); //dump new memory map
  //new int(12);
  fflush (stdout);
  //let's get some SEGVs
  //rkprintf("try to read 12\n"); fflush(stdout);
  printf("accessing pInt %d\n",*pInt);
  printf("reading again from pInt %d\n",*pInt);
  printf("try to write 33\n");
  *pInt=33;
  printf("read %d\n",*pInt);
  // String may have been allowed by prev. reads.  Lock it down again.
  assert(pac.forbidPage(pString));
  std::string strtmp = *pString;
  cout << "reading from string " << strtmp << endl;
  cout << "reading again from string " << *pString << endl;
  double xsecond = pPair->second;
  cout << "reading double from pair " << xsecond << endl;
  cout << "reading again double from pair " << pPair->second << endl;

  //restore default/old handler
  (void)sigaction(SIGSEGV,&stdSEGV,NULL);
  printf("try to read 33\n");
  printf("read %d\n",*pInt);
  PtrAccessSEGVHandler::const_iterator i(h.beginAccessedPtrs()),
    e(h.endAccessedPtrs());
  cout << "accessed ptrs" << endl;
  while (i != e) {
    cout << '@' << hex << *i++ << endl; 
  }
  pString->~string();
  delete [] pool;
  cout << "*** SEGVHandler_test OK ***" <<endl;
  return rc;
}
