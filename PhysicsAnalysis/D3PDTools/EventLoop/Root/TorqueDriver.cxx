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

#include <EventLoop/TorqueDriver.h>

#include <EventLoop/Job.h>
#include <EventLoop/JobSubmitInfo.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TSystem.h>
#include <sstream>

//
// method implementations
//

ClassImp(EL::TorqueDriver)

namespace EL
{
  void TorqueDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  TorqueDriver ::
  TorqueDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  std::string TorqueDriver ::
  batchJobId () const
  {
    RCU_READ_INVARIANT (this);
    return "EL_JOBID=$PBS_ARRAYID\n";
  }



  void TorqueDriver ::
  batchSubmit (Detail::JobSubmitInfo& info) const
  {
    RCU_READ_INVARIANT (this);

    if (info.resubmit)
      RCU_THROW_MSG ("resubmission not supported for this driver");

    assert (!info.batchJobIndices.empty());
    assert (info.batchJobIndices.back() + 1 == info.batchJobIndices.size());
    const std::size_t njob = info.batchJobIndices.size();

    std::ostringstream cmd;
    cmd << "cd " << info.submitDir << "/submit && qsub "
        << info.options.castString (Job::optSubmitFlags)
        << " -t 0-" << (njob-1) << " run";
    if (gSystem->Exec (cmd.str().c_str()) != 0)
      RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
  }
}
