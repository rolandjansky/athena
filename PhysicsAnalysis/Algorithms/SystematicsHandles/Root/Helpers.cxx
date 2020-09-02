/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/Helpers.h>

#include <RootCoreUtils/StringUtil.h>
#include <PATInterfaces/SystematicSet.h>
#include <regex>

//
// method implementations
//

namespace CP
{
  std::string nominalSystematicsName ()
  {
    return "NOSYS";
  }



  std::string makeSystematicsName (const std::string& name,
                                   const CP::SystematicSet& sys)
  {
    std::string sysName = sys.name();
    if (sysName.empty())
      sysName = nominalSystematicsName();
    return RCU::substitute (name, "%SYS%", sysName);
  }



  std::string makeSystematicsName (const std::string& name,
                                   const std::string& affecting,
                                   const CP::SystematicSet& sys)
  {
    CP::SystematicSet mysys;
    std::regex expr (affecting);

    for (auto& var : sys)
    {
      if (regex_match (var.basename(), expr))
        mysys.insert (var);
    }
    return makeSystematicsName (name, mysys);
  }
}
