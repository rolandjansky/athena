/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/LocalDriver.h>

#include <sstream>
#include <TSystem.h>
#include <AsgMessaging/StatusCode.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/ShellExec.h>
#include <RootCoreUtils/ThrowMsg.h>

//
// method implementations
//

ClassImp(EL::LocalDriver)

namespace EL
{
  void LocalDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  LocalDriver ::
  LocalDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  ::StatusCode LocalDriver ::
  doManagerStep (Detail::ManagerData& data) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;
    ANA_CHECK (BatchDriver::doManagerStep (data));
    switch (data.step)
    {
    case Detail::ManagerStep::batchScriptVar:
      {
        data.batchSkipReleaseSetup = true;
      }
      break;

    case Detail::ManagerStep::submitJob:
    case Detail::ManagerStep::doResubmit:
      {
        // safely ignoring: resubmit

        const std::string dockerImage {
          data.options.castString(Job::optDockerImage)};
        const std::string dockerOptions {
          data.options.castString(Job::optDockerOptions)};

        std::ostringstream basedirName;
        basedirName << data.submitDir << "/tmp";
        if (!data.resubmit)
        {
          if (gSystem->MakeDirectory (basedirName.str().c_str()) != 0)
            RCU_THROW_MSG ("failed to create directory " + basedirName.str());
        }
        for (std::size_t index : data.batchJobIndices)
        {
          std::ostringstream dirName;
          dirName << basedirName.str() << "/" << index;
          if (gSystem->MakeDirectory (dirName.str().c_str()) != 0)
            RCU_THROW_MSG ("failed to create directory " + dirName.str());

          std::ostringstream cmd;
          cmd << "cd " << dirName.str() << " && ";
          if (!dockerImage.empty())
            cmd << "docker run --rm -v " << RCU::Shell::quote (data.submitDir) << ":" << RCU::Shell::quote (data.submitDir) << " " << dockerOptions << " " << dockerImage << " ";
          cmd << RCU::Shell::quote (data.submitDir) << "/submit/run " << index;
          RCU::Shell::exec (cmd.str());
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
