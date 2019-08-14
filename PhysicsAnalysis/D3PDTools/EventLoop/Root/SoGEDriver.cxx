/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//          Copyright Nils Krumnack 2011
//                    Dennis Sperlich 2015.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (krumnack@iastate.edu / dsperlic@cern.ch)
// for bug reports, feature suggestions, praise and complaints.


//
// includes
//

#include <EventLoop/SoGEDriver.h>

#include <AsgTools/StatusCode.h>
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



  std::string SoGEDriver ::
  batchJobId () const
  {
    RCU_READ_INVARIANT (this);
    return "EL_JOBID=$(($SGE_TASK_ID-1))\n";
  }



  ::StatusCode SoGEDriver ::
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
