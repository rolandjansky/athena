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
#include <EventLoop/Job.h>
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



  void LocalDriver ::
  batchSubmit (const std::string& location, const SH::MetaObject& /*options*/,
	       std::size_t njob) const
  {
    RCU_READ_INVARIANT (this);

    std::ostringstream basedirName;
    basedirName << location << "/tmp";
    if (gSystem->MakeDirectory (basedirName.str().c_str()) != 0)
      RCU_THROW_MSG ("failed to create directory " + basedirName.str());
    for (std::size_t index = 0; index != njob; ++ index)
    {
      std::ostringstream dirName;
      dirName << basedirName.str() << "/" << index;
      if (gSystem->MakeDirectory (dirName.str().c_str()) != 0)
	RCU_THROW_MSG ("failed to create directory " + dirName.str());

      std::ostringstream cmd;
      cmd << "cd " << dirName.str() << " && ";
#ifndef USE_CMAKE
      if (!options.castBool (Job::optLocalNoUnsetup, false))
	cmd << " source $ROOTCOREDIR/scripts/unsetup.sh && ";
#endif
      cmd << location << "/submit/run " << index;
      RCU::Shell::exec (cmd.str());
    }
  }
}
