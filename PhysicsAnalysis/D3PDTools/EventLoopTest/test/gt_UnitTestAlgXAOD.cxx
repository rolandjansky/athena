/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <EventLoopTest/UnitTestAlgXAOD.h>
#include <AsgTesting/UnitTest.h>
#include <AthContainers/AuxStoreStandalone.h>
#include <AsgTools/StatusCode.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <SampleHandler/SampleLocal.h>
#include <EventLoop/DirectDriver.h>
#include <EventLoop/Job.h>
#include <EventLoop/LocalDriver.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <xAODEventInfo/EventInfo.h>
#include <xAODEventInfo/EventAuxInfo.h>
#include <xAODRootAccess/Init.h>
#include <xAODRootAccess/TEvent.h>
#include <xAODRootAccess/TStore.h>
#include <cstdlib>

using namespace EL;

//
// unit test
//

TEST (UnitTestAlgXAODTest, all_tests)
{
  xAOD::TReturnCode::enableFailure();
  xAOD::Init ().ignore();

  std::string prefix = "UnitTestAlgXAODSubmit";
  RCU::Shell::exec ("rm -rf " + prefix + "*");

  const char *ASG_TEST_FILE_DATA = getenv ("ASG_TEST_FILE_DATA");
  ASSERT_NE (ASG_TEST_FILE_DATA, nullptr);
  const char *ASG_TEST_FILE_MC = getenv ("ASG_TEST_FILE_MC");
  ASSERT_NE (ASG_TEST_FILE_MC, nullptr);

  std::unique_ptr<SH::SampleLocal> sample (new SH::SampleLocal ("sample"));
  sample->add (ASG_TEST_FILE_DATA);
  sample->add (ASG_TEST_FILE_MC);
  SH::SampleHandler sh;
  sh.add (sample.release());

  {
    Job job;
    auto alg = std::make_unique<UnitTestAlgXAOD>();
    job.algsAdd (alg.release());
    job.sampleHandler (sh);

    {
      DirectDriver driver;
      driver.submit (job, prefix);
    }
  }
}

ATLAS_GOOGLE_TEST_MAIN
