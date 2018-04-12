/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011.
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

    // name of tarball being made (this needs to match BatchDriver.cxx)
    const std::string tarballName("AnalysisPackage.tar.gz");

    if(!options.castBool(Job::optBatchSharedFileSystem,true))
    {
      const std::string newLocation = location + "/submit/" + tarballName;
      int status=gSystem->CopyFile(tarballName.c_str(),newLocation.c_str());
      if(status != 0)
      RCU_THROW_MSG( ("failed to copy " + tarballName + " to " + newLocation).c_str() );
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
	  file << "transfer_input_files    = submit/" << tarballName << ", submit/segments, submit/config.root\n";
	  file << "transfer_output_files   = fetch, status\n";
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
