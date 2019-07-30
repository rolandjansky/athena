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

#include <EventLoop/GEDriver.h>

#include <EventLoop/Job.h>
#include <EventLoop/JobSubmitInfo.h>
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



  void GEDriver ::
  batchSubmit (Detail::JobSubmitInfo& info, const SH::MetaObject& options,
               const std::vector<std::size_t>& jobIndices)
    const
  {
    RCU_READ_INVARIANT (this);

    // safely ignoring: resubmit

    std::ostringstream cmd;
    cmd << "cd " << info.submitDir << "/submit";
    for (std::size_t iter : jobIndices)
    {
      cmd << " && qsub " << options.castString (Job::optSubmitFlags)
          << " -o " << info.submitDir << "/submit/log-" << iter << ".out"
          << " -e " << info.submitDir << "/submit/log-" << iter << ".err"
          << " run " << iter;
    }
    if (gSystem->Exec (cmd.str().c_str()) != 0)
      RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
  }
}
