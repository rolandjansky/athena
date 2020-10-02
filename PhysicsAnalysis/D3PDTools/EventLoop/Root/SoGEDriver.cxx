/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Dennis Sperlich 2015
/// @author Nils Krumnack


//
// includes
//

#include <EventLoop/SoGEDriver.h>

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

ClassImp(EL::SoGEDriver)

namespace EL
{
  void SoGEDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  SoGEDriver ::
  SoGEDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  ::StatusCode SoGEDriver ::
  doManagerStep (Detail::ManagerData& data) const
  {
    RCU_READ_INVARIANT (this);
    using namespace msgEventLoop;
    ANA_CHECK (BatchDriver::doManagerStep (data));
    switch (data.step)
    {
    case Detail::ManagerStep::batchScriptVar:
      {
        data.batchJobId = "EL_JOBID=$(($SGE_TASK_ID-1))\n";
      }
      break;

    case Detail::ManagerStep::submitJob:
    case Detail::ManagerStep::doResubmit:
      {
        if (data.resubmit)
          RCU_THROW_MSG ("resubmission not supported for this driver");

        assert (!data.batchJobIndices.empty());
        assert (data.batchJobIndices.back() + 1 == data.batchJobIndices.size());
        const std::size_t njob = data.batchJobIndices.size();

        std::ostringstream cmd;
        cmd << "cd " << data.submitDir << "/submit && qsub "
            << data.options.castString (Job::optSubmitFlags)
            << " -t 1-" << (njob) << " run";
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
