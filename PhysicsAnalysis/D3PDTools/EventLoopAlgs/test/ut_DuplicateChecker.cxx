/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoopAlgs/DuplicateChecker.h>
#include <EventLoopAlgs/Global.h>

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

/// \brief make an xAOD with duplicate events
::StatusCode makeXAOD (const std::string& file, unsigned runNumber,
	       unsigned firstEventNumber)
{
  xAOD::TEvent event;
  xAOD::TStore store;

  // so apparently I need an input file to make an output file...
  std::unique_ptr<TFile> inputFile (TFile::Open (getenv ("ROOTCORE_TEST_FILE"), "READ"));
  RCU_ASSERT (inputFile != nullptr);
  TTree *tree = dynamic_cast<TTree*>(inputFile->Get ("CollectionTree"));
  RCU_ASSERT (tree);
  if (event.readFrom (tree).isFailure())
    RCU_ASSERT0 ("failed to read from input tree");
  std::unique_ptr<TFile> myfile (TFile::Open (file.c_str(), "RECREATE"));
  if (event.writeTo (myfile.get()).isFailure())
  {
    std::cout << "failed to write to file" << std::endl;
    return ::StatusCode::FAILURE;
  }
   


  for (unsigned iter = 0, end = 9000; iter != end; ++ iter)
  {
    unsigned duplicates = 1;
    if (iter % 9 == 0)
      ++ duplicates;
    for (unsigned jter = 0; jter != duplicates; ++ jter)
    {
      store.clear ();
      event.getEntry (0);

      std::unique_ptr<xAOD::EventInfo> info (new xAOD::EventInfo);
      std::unique_ptr<xAOD::EventAuxInfo> aux (new xAOD::EventAuxInfo);
      info->setStore (aux.get());
      info->setRunNumber (runNumber);
      info->setEventNumber (firstEventNumber + iter);
      if (event.record (info.release(), "MyEventInfo").isFailure())
      {
	std::cout << "failed to record EventInfo" << std::endl;
	return ::StatusCode::FAILURE;
      }
      if (event.record (aux.release(), "MyEventInfoAux.").isFailure())
      {
      	std::cout << "failed to record EventInfoAux" << std::endl;
      	return ::StatusCode::FAILURE;
      }
      if (event.fill () < 0)
      {
	std::cout << "failed to write to file" << std::endl;
	return ::StatusCode::FAILURE;
      }
    }
  }
  if(event.finishWritingTo(myfile.get()).isFailure())
  {
    std::cout << "failed to finish writing to file" << std::endl;
    return ::StatusCode::FAILURE;
  }
  myfile->Write ();
  return ::StatusCode::SUCCESS;
}

/// \brief check the histogram output
/// \par Guarantee
///   basic
/// \par Failures
///   unit test failures\n
///   i/o errors
void checkHistograms (const std::string& submitdir,
		      unsigned raw, unsigned final,
		      bool expect_success)
{
  std::unique_ptr<TFile> file (TFile::Open ((submitdir + "/hist-sample.root").c_str(), "READ"));
  RCU_ASSERT (file != nullptr);

  TH1 *hist = dynamic_cast<TH1*>(file->Get ("EventLoop_EventCount"));
  RCU_ASSERT (file != nullptr);

  // hist->Print ("ALL");
  RCU_ASSERT (hist->GetBinContent(1) == raw);
  RCU_ASSERT (hist->GetBinContent(2) == raw);
  RCU_ASSERT (hist->GetBinContent(3) == final);

  TTree *summary = dynamic_cast<TTree*>(file->Get ("summary"));
  RCU_ASSERT (summary != nullptr);

  // summary->Print ("ALL");
  RCU_ASSERT (summary->GetEntries() == raw);

  summary->SetBranchStatus ("*", 0);
  summary->SetBranchStatus ("processed", 1);
  Bool_t processed = false;
  summary->SetBranchAddress ("processed", &processed);
  unsigned count = 0;
  for (unsigned iter = 0; iter != raw; ++ iter)
  {
    summary->GetEntry (iter);
    if (processed)
      ++ count;
  }
  RCU_ASSERT (count == final);

  bool success = DuplicateChecker::processSummary (submitdir, "summary");
  RCU_ASSERT (success == expect_success);
}

int main ()
{
  xAOD::TReturnCode::enableFailure();
  xAOD::Init ().ignore();

  std::string prefix = "DuplicateCheckerSubmit";

  RCU::Shell::exec ("rm -rf " + prefix + "[123]");

  if (makeXAOD ("test1.root", 1, 0).isFailure())
  {
    std::cout << "failed to make test file" << std::endl;
    return EXIT_FAILURE;
  }
  if (makeXAOD ("test2.root", 1, 8000).isFailure())
  {
    std::cout << "failed to make test file" << std::endl;
    return EXIT_FAILURE;
  }
  if (makeXAOD ("test3.root", 2, 0).isFailure())
  {
    std::cout << "failed to make test file" << std::endl;
    return EXIT_FAILURE;
  }
  std::unique_ptr<SH::SampleLocal> sample (new SH::SampleLocal ("sample"));
  sample->add ("test1.root");
  sample->add ("test2.root");
  sample->add ("test3.root");
  SH::SampleHandler sh;
  sh.add (sample.release());

  {
    Job job;
    std::unique_ptr<DuplicateChecker> alg (new DuplicateChecker);
    alg->setEventInfoName ("MyEventInfo");
    alg->setOutputTreeName ("summary");
    job.algsAdd (alg.release());
    job.sampleHandler (sh);

    {
      DirectDriver driver;
      driver.submit (job, prefix + "1");
      checkHistograms (prefix + "1", 30000, 26000, true);
    }
    {
      LocalDriver driver;
      driver.submit (job, prefix + "2");
      checkHistograms (prefix + "2", 30000, 27000, false);
    }
    RCU::Shell::exec ("cmp " + prefix + "1/duplicates " + prefix + "2/duplicates");
  }

  {
    Job job;
    std::unique_ptr<DuplicateChecker> alg (new DuplicateChecker);
    alg->setEventInfoName ("MyEventInfo");
    alg->setOutputTreeName ("summary");
    alg->addKnownDuplicatesFile (prefix + "1/duplicates");
    job.algsAdd (alg.release());
    job.sampleHandler (sh);

    {
      LocalDriver driver;
      driver.submit (job, prefix + "3");
      checkHistograms (prefix + "3", 30000, 26000, true);
    }
  }

  return 0;
}
