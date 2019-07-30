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

#include <EventLoop/LSFDriver.h>

#include <EventLoop/Job.h>
#include <EventLoop/JobSubmitInfo.h>
#include <RootCoreUtils/ThrowMsg.h>
#include <TSystem.h>
#include <sstream>

//
// method implementations
//

ClassImp(EL::LSFDriver)

namespace EL
{
  void LSFDriver ::
  testInvariant () const
  {
    RCU_INVARIANT (this != 0);
  }



  LSFDriver ::
  LSFDriver ()
  {
    RCU_NEW_INVARIANT (this);
  }



  void LSFDriver ::
  batchSubmit (Detail::JobSubmitInfo& info) const
  {
    RCU_READ_INVARIANT (this);

    // safely ignoring: resubmit

    std::ostringstream cmd;
    cmd << "cd " << info.submitDir << "/submit";
    for (std::size_t iter : info.batchJobIndices)
    {
      cmd << " && bsub " << info.options.castString (Job::optSubmitFlags);
      if (info.options.castBool (Job::optResetShell, true))
        cmd << " -L /bin/bash";
      cmd << " " << info.submitDir << "/submit/run " << iter;
    }
    if (gSystem->Exec (cmd.str().c_str()) != 0)
      RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
  }
}
