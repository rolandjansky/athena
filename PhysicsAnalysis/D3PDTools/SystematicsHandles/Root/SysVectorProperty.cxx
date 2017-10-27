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

#include <SystematicsHandles/SysVectorProperty.h>

#include <AsgTools/StatusCode.h>
#include <PATInterfaces/SystematicSet.h>
#include <RootCoreUtils/Assert.h>

//
// method implementations
//

namespace EL
{
  void SysVectorProperty ::
  addAffectingSystematics (const CP::SystematicSet& /*affectingSystematics*/)
  {
    assert (!isInitialized());
    // no-op
  }



  ::StatusCode SysVectorProperty ::
  initialize ()
  {
    // take an empty property as running just the central systematics
    // set.  implication is that you can't really run the algorithm
    // with it doing nothing, but that ought to be Ok.
    if (m_systematicsListProperty.empty())
    {
      m_systematicsVector.push_back (CP::SystematicSet ());
      return StatusCode::SUCCESS;
    }

    // for now let's just take the list of configured systematics as
    // is.
    for (const std::string& sysName : m_systematicsListProperty)
    {
      m_systematicsVector.push_back (CP::SystematicSet (sysName));
    }
    return StatusCode::SUCCESS;
  }
}
