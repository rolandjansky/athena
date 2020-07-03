/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  AthenaServices/test/FPEControlSvc_test.cxx
 * @author scott snyder
 * @date Nov 2007
 * @brief Regression test for FPEControlSvc
 */


#undef NDEBUG


#include "TestTools/initGaudi.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h"
#include <iostream>
#include <cassert>
#include <signal.h>
#include <setjmp.h>


float blam = 0;
jmp_buf out;


void sighandler (int)
{
  siglongjmp (out, 1);
}


float testit (bool expectsig)
{
  // Declaring this volatile avoids a warning that longjmp
  // might clobber it.
  volatile float x = 1;
  signal (SIGFPE, sighandler);
  if (sigsetjmp (out, 1) == 0) {
    x = x / blam;
    assert (!expectsig);
  }
  else
    assert (expectsig);
  signal (SIGFPE, SIG_DFL);
  return x;
}


int main()
{
  errorcheck::ReportMessage::hideErrorLocus();
  ISvcLocator* svcloc = nullptr;
  if (!Athena_test::initGaudi("AthenaServices/FPEControlSvc_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  
  assert(svcloc);

  IService *tmpsvc = nullptr;
  if (svcloc->service ("FPEControlSvc", tmpsvc).isFailure()) std::abort();
  Service* svc = dynamic_cast<Service*> (tmpsvc);

  std::vector<std::string> flags;

  //  flags.push_back ("divbyzero");
  //  svc->setProperty ("Exceptions", flags);
  testit (true);

  flags.push_back ("!divbyzero");
  assert(svc->setProperty ("Exceptions", flags).isSuccess());
  testit (false);

  flags.push_back ("divbyzero");
  assert(svc->setProperty ("Exceptions", flags).isSuccess());
  testit (true);

  flags.push_back ("fooo");
  assert(svc->setProperty ("Exceptions", flags).isSuccess());
  
  return 0;
}
