/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/GEDriver.h>

#include <AsgMessaging/StatusCode.h>
#include <EventLoop/Job.h>
#include <EventLoop/ManagerData.h>
#include <EventLoop/MessageCheck.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TSystem.h>
#include <sstream>

//
// method implementations
//

ClassImp(EL::GEDriver)

namespace EL
{
  void GEDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  GEDriver ::
  GEDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  ::StatusCode GEDriver ::
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

        std::ostringstream cmd;
        cmd << "cd " << data.submitDir << "/submit";
        for (std::size_t iter : data.batchJobIndices)
        {
          cmd << " && qsub " << data.options.castString (Job::optSubmitFlags)
              << " -o " << data.submitDir << "/submit/log-" << iter << ".out"
              << " -e " << data.submitDir << "/submit/log-" << iter << ".err"
              << " run " << iter;
        }
        if (gSystem->Exec (cmd.str().c_str()) != 0)
          RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
        data.submitted = true;
      }
      break;

    default:
      break;
    }
    return ::StatusCode::SUCCESS;
  }
}
