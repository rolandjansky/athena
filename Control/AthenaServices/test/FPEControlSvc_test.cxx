/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FPEControlSvc_test.cxx,v 1.3 2008-12-08 22:02:46 calaf Exp $
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
  ISvcLocator* svcloc = 0;
  if (!Athena_test::initGaudi("FPEControlSvc_test.txt", svcloc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }  
  assert(svcloc);

  IService *tmpsvc = 0;
  if (svcloc->service ("FPEControlSvc", tmpsvc).isFailure()) std::abort();
  Service* svc = dynamic_cast<Service*> (tmpsvc);

  std::vector<std::string> flags;

  //  flags.push_back ("divbyzero");
  //  svc->setProperty ("Exceptions", flags);
  testit (true);

  flags.push_back ("!divbyzero");
  svc->setProperty ("Exceptions", flags);
  testit (false);

  flags.push_back ("divbyzero");
  svc->setProperty ("Exceptions", flags);
  testit (true);

  flags.push_back ("fooo");
  svc->setProperty ("Exceptions", flags);
  
  return 0;
}
