/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/CondorDriver.h>

#include <EventLoop/BatchJob.h>
#include <EventLoop/Job.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TSystem.h>
#include <fstream>
#include <memory>
#include <sstream>

//
// method implementations
//

ClassImp(EL::CondorDriver)

namespace EL
{
  void CondorDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  CondorDriver ::
  CondorDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }


  std::string CondorDriver ::
  batchInit () const
  {
    RCU_READ_INVARIANT (this);
    return "export PATH LD_LIBRARY_PATH PYTHONPATH";
  }

  void CondorDriver ::
  batchSubmit (const std::string& location, const SH::MetaObject& options,
	       std::size_t njob) const
  {
    RCU_READ_INVARIANT (this);

#ifdef USE_CMAKE
    const char* AB_SETUP = getenv("AnalysisBase_SET_UP");
    const char* AT_SETUP = getenv("AnalysisTop_SET_UP");
    bool rel_AnalysisBase(false);
    bool rel_AnalysisTop(false);
    if(AB_SETUP) rel_AnalysisBase = strcmp(AB_SETUP, "1") == 0;
    if(AT_SETUP) rel_AnalysisBase = strcmp(AT_SETUP, "1") == 0;
    if(rel_AnalysisBase && rel_AnalysisTop) RCU_THROW_MSG("AnalysisBase_SET_UP and AnalysisTop_SET_UP both seem to be set to 1.");
    if(!rel_AnalysisBase && !rel_AnalysisTop) RCU_THROW_MSG("AnalysisBase_SET_UP and AnalysisTop_SET_UP both seem to be set to 0.");

    const char *ANALYSIS_PLATFORM = nullptr;
    if(rel_AnalysisBase) ANALYSIS_PLATFORM = getenv ("AnalysisBase_PLATFORM");
    if(rel_AnalysisTop)  ANALYSIS_PLATFORM = getenv ("AnalysisTop_PLATFORM");
#endif

    if(!options.castBool(Job::optBatchSharedFileSystem,true))
    {
#ifndef USE_CMAKE
      const std::string fileName_tarball = "RootCore.par";
#else
      const std::string fileName_tarball = ANALYSIS_PLATFORM + std::string(".tar.gz");
#endif
      const std::string newLocation = location + "/submit/" + fileName_tarball;
      int status=gSystem->CopyFile(fileName_tarball.c_str(),newLocation.c_str());
      if(status != 0)
      RCU_THROW_MSG( ("failed to copy " + fileName_tarball + " to " + newLocation).c_str() );
    }

    {
      std::ofstream file ((location + "/submit/submit").c_str());
      file << "executable              = run\n";
      file << "universe                = vanilla\n";
      file << "log                     = submit/run.log\n";
      file << "output                  = submit/log-$(Process).out\n";
      file << "error                   = submit/log-$(Process).err\n";
      file << "initialdir              = " << location << "\n";
      if(!options.castBool(Job::optBatchSharedFileSystem,true))
	{ // Transfer data with non-shared file-systems
	  file << "should_transfer_files   = YES\n";
	  file << "when_to_transfer_output = ON_EXIT\n";
#ifndef USE_CMAKE
	  file << "transfer_input_files    = submit/RootCore.par, submit/segments, submit/config.root\n";
#else
	  file << "transfer_input_files    = submit/" << ANALYSIS_PLATFORM << ".tar.gz, submit/segments, submit/config.root\n";
#endif
	  file << "transfer_output_files   = fetch\n";
	  file << "x509userproxy           = " << gSystem->Getenv("X509_USER_PROXY") <<"\n";
	}
      file << "arguments               = $(Process)\n";
      file << "\n" << options.castString (Job::optCondorConf) << "\n";
      file << "queue " << njob << "\n";
    }

    {
      std::ostringstream cmd;
      cmd << "cd " << location << "/submit && condor_submit "
	  << options.castString (Job::optSubmitFlags) << " submit";
      if (gSystem->Exec (cmd.str().c_str()) != 0)
	RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
    }
  }
}
