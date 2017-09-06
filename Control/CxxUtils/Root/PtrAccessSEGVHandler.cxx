/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define DEBUG 1

#include "CxxUtils/PtrAccessSEGVHandler.h"
#include "CxxUtils/PageAccessControl.h"
#include <cstdio>
#include <cstdlib>

void
PtrAccessSEGVHandler::handle(int /*signal_number*/,siginfo_t *sigi,void * /*unused*/) {
    void *addr=sigi->si_addr;
#ifdef DEBUG
    printf("page fault @address=%p\n",sigi->si_addr);
#ifdef __ARCH_SI_TRAPNO
    printf("page fault trapno=%d\n",sigi->si_trapno);
#endif
    printf("page fault signo=%d\n",sigi->si_signo);
    printf("page fault errno=%d\n",sigi->si_errno);
    printf("this page fault failed because ");
    if (SEGV_MAPERR == sigi->si_code) 
      printf("you tried to access an invalid address\n");
    else if (SEGV_ACCERR == sigi->si_code) {
      printf("you tried to access a protected address\n");
    }  else printf(" an unknown reason. Page fault code=%d\n",sigi->si_code);
#endif
    //record the access and restore page protection
    if (SEGV_ACCERR == sigi->si_code) {
      m_pac.restorePageProt(addr);
      m_accessed.push_back(addr);
    } else abort();
}
