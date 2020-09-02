/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SysListType.h>

#include <string>

//
// method implementations
//

namespace CP
{
  const std::string& sysListDefaultName ()
  {
    static const std::string result = "systematics";
    return result;
  }
}
