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
  batchSubmit (const std::string& location, const SH::MetaObject& options,
	       std::size_t njob) const
  {
    RCU_READ_INVARIANT (this);

    {
      std::ostringstream cmd;
      cmd << "cd " << location << "/submit";
      for (unsigned iter = 0, end = njob; iter != end; ++ iter)
      {
	cmd << " && bsub " << options.castString (Job::optSubmitFlags);
	if (options.castBool (Job::optResetShell, true))
	  cmd << " -L /bin/bash";
	cmd << " " << location << "/submit/run " << iter;
      }
      if (gSystem->Exec (cmd.str().c_str()) != 0)
	RCU_THROW_MSG (("failed to execute: " + cmd.str()).c_str());
    }
  }
}
