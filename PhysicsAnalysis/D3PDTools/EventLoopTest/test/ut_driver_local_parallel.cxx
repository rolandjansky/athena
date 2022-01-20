/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


//
// includes
//

#include <EventLoop/Global.h>

#include <EventLoop/LocalDriver.h>
#include <EventLoop/Job.h>
#include <EventLoopTest/UnitTest.h>

//
// main program
//

using namespace EL;

int main ()
{
  LocalDriver driver;
  driver.options()->setDouble (Job::optNumParallelProcs, 0u);
  UnitTest ut ("local");
  ut.cleanup = false;
  return ut.run (driver);
}
