/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


//
// includes
//

#include <EventLoop/Global.h>

#include <EventLoopGrid/PrunDriver.h>
#include <EventLoopTest/UnitTest.h>
#include <RootCoreUtils/ShellExec.h>
#include <SampleHandler/GridTools.h>
#include <sstream>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

//
// main program
//

using namespace EL;

int main ()
{
  // can't use my current directory, it may easily overflow
  std::ostringstream stageDir;
  {
    const char *TMPDIR = getenv ("TMPDIR");
    if (TMPDIR)
      stageDir << TMPDIR << "/";
    else
      stageDir << "/tmp/";
  }
  stageDir << "GridToolsTest." << getpid();
  RCU::Shell::exec ("rm -rf " + RCU::Shell::quote (stageDir.str()));
  RCU::Shell::exec ("mkdir -p " + RCU::Shell::quote (stageDir.str()));
  setenv (SH::downloadStageEnvVar().c_str(), stageDir.str().c_str(), 0);

  PrunDriver driver;
  std::ostringstream output;
  output << "user.krumnack.EventLoopTest.output." << time(nullptr) << ".%in:name[6]%";
  driver.options()->setString("nc_outputSampleName", output.str());

  UnitTest ut ("grid");
  ut.gridInput = true;
  // ut.cleanup = false;
  return ut.run (driver);
}
