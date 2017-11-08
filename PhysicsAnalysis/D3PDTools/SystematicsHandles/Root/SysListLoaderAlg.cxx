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

#include <SystematicsHandles/SysListLoaderAlg.h>

//
// method implementations
//

namespace EL
{
  SysListLoaderAlg :: 
  SysListLoaderAlg (const std::string& name, 
                    ISvcLocator* pSvcLocator)
    : AnaAlgorithm (name, pSvcLocator)
  {
    declareProperty ("systematicsName", m_systematicsName, "the name of the systematics in the event store");
    declareProperty ("systematicsList", m_systematicsList, "the list of systematics to run");
  }



  StatusCode SysListLoaderAlg ::
  initialize ()
  {
    if (m_systematicsName.empty())
    {
      ANA_MSG_ERROR ("no name configured for the systematics list");
      return StatusCode::FAILURE;
    }

    // take an empty property as running just the central systematics
    // set.  implication is that you can't really run doing nothing,
    // but that ought to be Ok.
    if (m_systematicsList.empty())
    {
      m_systematicsVector.push_back (CP::SystematicSet ());
    } else
    {
      for (const std::string& sysName : m_systematicsList)
        m_systematicsVector.push_back (CP::SystematicSet (sysName));
    }
    return StatusCode::SUCCESS;
  }



  StatusCode SysListLoaderAlg ::
  execute ()
  {
    std::unique_ptr<SysListType> list (new SysListType (m_systematicsVector));
    evtStore()->record (list.release(), m_systematicsName);
    return StatusCode::SUCCESS;
  }
}
