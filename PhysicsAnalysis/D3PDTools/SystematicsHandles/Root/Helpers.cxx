//        Copyright Iowa State University 2017.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


//
// includes
//

#include <SystematicsHandles/Helpers.h>

#include <RootCoreUtils/Assert.h>
#include <RootCoreUtils/StringUtil.h>
#include <PATInterfaces/SystematicSet.h>

//
// method implementations
//

namespace EL
{
  namespace detail
  {
    std::string makeSystematicsName (const std::string& name,
                                     const CP::SystematicSet& sys)
    {
      std::string sysName = sys.name();
      if (sysName.empty())
        sysName = "NOSYS";
      return RCU::substitute (name, "%SYS%", sysName);
    }
  }
}
