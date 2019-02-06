/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#include <PATInterfaces/SystematicsUtil.h>

#include <PATInterfaces/MakeSystematicsVector.h>
#include <PATInterfaces/SystematicSet.h>
#include <map>

//
// method implementations
//

namespace CP
{
  std::vector<SystematicSet>
  make_systematics_vector (const SystematicSet& systematics)
  {
    MakeSystematicsVector sys;
    sys.calc (systematics);
    return sys.result("");
  }
}
