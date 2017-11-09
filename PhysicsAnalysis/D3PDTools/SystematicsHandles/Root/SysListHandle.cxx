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

#include <SystematicsHandles/SysListHandle.h>

#include <AsgTools/MessageCheck.h>
#include <AsgTools/StatusCode.h>
#include <PATInterfaces/SystematicSet.h>
#include <RootCoreUtils/Assert.h>

#include <stdexcept>

//
// method implementations
//

namespace EL
{
  void SysListHandle ::
  addAffectingSystematics (const CP::SystematicSet& /*affectingSystematics*/)
  {
    assert (!isInitialized());
    // no-op
  }



  ::StatusCode SysListHandle ::
  initialize ()
  {
    m_evtStore = m_evtStoreGetter();
    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }



  const std::vector<CP::SystematicSet>& SysListHandle ::
  systematicsVector () const
  {
    assert (isInitialized());
    const SysListType *systematicsList = nullptr;
#ifdef ROOTCORE
    ANA_CHECK_THROW (m_evtStore->retrieve (systematicsList, m_systematicsListName));
    return *systematicsList;
#else
    throw std::runtime_error ("reading the systematics list from the event store is currently not supported");
#endif
  }
}
