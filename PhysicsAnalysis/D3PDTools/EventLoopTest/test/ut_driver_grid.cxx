/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//
// includes
//

#include <EventLoop/Global.h>

#include <EventLoopGrid/PrunDriver.h>
#include <EventLoopTest/UnitTest.h>
#include <sstream>
#include <time.h>

//
// main program
//

using namespace EL;

int main ()
{
  PrunDriver driver;
  std::ostringstream output;
  output << "user.krumnack.EventLoopTest.output." << time(nullptr) << ".%in:name[6]%";
  driver.options()->setString("nc_outputSampleName", output.str());

  UnitTest ut ("grid");
  ut.gridInput = true;
  // ut.cleanup = false;
  // ut.location = "$HOME/unit-test.$$";
  return ut.run (driver);
}
