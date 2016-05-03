//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


//
// includes
//

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
