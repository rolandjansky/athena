/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <RootCoreUtils/CheckRootVersion.h>

#include <cstdlib>
#include <iostream>
#include <TROOT.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace RCU
{
  namespace
  {
    bool checked = false;
  }

  void check_root_version ()
  {
    if (checked)
      return;
    if (gROOT->GetVersionCode() != Int_t(ROOT_VERSION_CODE))
    {
      std::cout
	<< "root version used is not compatible with the one used for\n"
	<< "compilation.  please load either the correct root version, or\n"
	<< "recompile with the new version\n\n"
	<< "alternatively deactivate this check using:\n"
	<< "#include <RootCoreUtils/CheckRootVersion.hh>\n"
	<< "  RCU::disable_root_version_check()\n"
	<< std::flush;
      std::abort ();
    }
    checked = true;
  }



  void disable_root_version_check ()
  {
    checked = true;
  }
}
