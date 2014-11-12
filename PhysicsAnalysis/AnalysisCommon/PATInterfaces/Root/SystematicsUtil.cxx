//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


//
// includes
//

#include <PATInterfaces/SystematicsUtil.h>

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
    std::map<std::string,std::vector<SystematicVariation> > basesys;

    for (auto sys : systematics)
    {
      basesys[sys.basename()].push_back (sys);
    }

    std::vector<CP::SystematicSet> result;

    // this is the nominal set
    result.push_back(CP::SystematicSet());

    for (auto sys : basesys)
    {
      bool isContinuous = false;
      for (auto mysys : sys.second)
      {
	if (mysys == CP::SystematicVariation (sys.first,
					      SystematicVariation::CONTINUOUS))
	  isContinuous = true;
      }

      if (isContinuous)
      {
	// for continuous systematics just evaluate +/-1 sigma
	result.push_back(CP::SystematicSet());
	result.back().insert (CP::SystematicVariation (sys.first, 1));
	result.push_back(CP::SystematicSet());
	result.back().insert (CP::SystematicVariation (sys.first, -1));
      } else
      {
	// otherwise just add all of them flat
	for (auto mysys : sys.second)
	{
	  result.push_back(CP::SystematicSet());
	  result.back().insert(mysys);
	}
      }
    }
    return result;
  }
}
