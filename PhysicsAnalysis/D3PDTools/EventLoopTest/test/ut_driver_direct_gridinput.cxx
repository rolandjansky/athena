/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


//
// includes
//

#include <EventLoop/Global.h>

#include <EventLoop/DirectDriver.h>
#include <EventLoopTest/UnitTest.h>

//
// main program
//

using namespace EL;

int main ()
{
  DirectDriver driver;
  UnitTest ut ("direct_gridinput");
  ut.gridInput = true;
  // ut.cleanup = false;
  return ut.run (driver);
}
