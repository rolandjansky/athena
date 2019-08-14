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

#include <EventLoop/LocalDriver.h>

#include <sstream>
#include <TSystem.h>
#include <AsgTools/StatusCode.h>
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



  std::string LocalDriver ::
  batchReleaseSetup (bool /*sharedFileSystem*/) const
  {
    RCU_READ_INVARIANT (this);
    return "";
  }



  ::StatusCode LocalDriver ::
  doManagerStep (Detail::ManagerData& data) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;
    ANA_CHECK (BatchDriver::doManagerStep (data));
    switch (data.step)
    {
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
