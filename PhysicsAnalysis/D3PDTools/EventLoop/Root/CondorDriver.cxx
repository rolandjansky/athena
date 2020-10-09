/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/CondorDriver.h>

#include <AsgMessaging/StatusCode.h>
#include <EventLoop/BatchJob.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/MessageCheck.h>
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



  ::StatusCode CondorDriver ::
  doManagerStep (Detail::ManagerData& data) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;
    ANA_CHECK (BatchDriver::doManagerStep (data));
    switch (data.step)
    {
    case Detail::ManagerStep::batchScriptVar:
      {
        data.batchInit = "export PATH LD_LIBRARY_PATH PYTHONPATH";
      }
      break;

    case Detail::ManagerStep::submitJob:
    case Detail::ManagerStep::doResubmit:
      {
        // name of tarball being made (this needs to match BatchDriver.cxx)
        const std::string tarballName("AnalysisPackage.tar.gz");

        if (!data.resubmit)
        {
          if(!data.options.castBool(Job::optBatchSharedFileSystem,true))
          {
            const std::string newLocation = data.submitDir + "/submit/" + tarballName;
            int status=gSystem->CopyFile(tarballName.c_str(),newLocation.c_str());
            if(status != 0)
              RCU_THROW_MSG( ("failed to copy " + tarballName + " to " + newLocation).c_str() );
          }
        }

        {
          std::ofstream file ((data.submitDir + "/submit/submit").c_str());
          file << "executable              = run\n";
          file << "universe                = vanilla\n";
          file << "log                     = submit/run.log\n";
          file << "output                  = submit/log-$(Item).out\n";
          file << "error                   = submit/log-$(Item).err\n";
          file << "initialdir              = " << data.submitDir << "\n";
          if(!data.options.castBool(Job::optBatchSharedFileSystem,true))
          { // Transfer data with non-shared file-systems
            file << "should_transfer_files   = YES\n";
            file << "when_to_transfer_output = ON_EXIT\n";
            file << "transfer_input_files    = submit/" << tarballName << ", submit/segments, submit/config.root\n";
            file << "transfer_output_files   = fetch, status\n";
            file << "x509userproxy           = " << gSystem->Getenv("X509_USER_PROXY") <<"\n";
          }
          file << "arguments               = $(Item)\n";
          file << "\n" << data.options.castString (Job::optCondorConf) << "\n";
          file << "queue in ( ";
          bool first {true};
          for (std::size_t index : data.batchJobIndices)
          {
            if (first)
              first = false;
            else
              file << ", ";
            file << index;
          }
          file << " )\n";
        }

        {
          std::ostringstream cmd;
          cmd << "cd " << data.submitDir << "/submit && condor_submit "
              << data.options.castString (Job::optSubmitFlags) << " submit";
          if (gSystem->Exec (cmd.str().c_str()) != 0)
            RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
        }
        data.submitted = true;
      }
      break;

    default:
      break;
    }
    return ::StatusCode::SUCCESS;
  }
}
