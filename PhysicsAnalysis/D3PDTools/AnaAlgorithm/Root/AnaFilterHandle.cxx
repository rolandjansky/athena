/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AnaAlgorithm/AnaFilterHandle.h>

#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/StatusCode.h>

//
// method implementations
//

namespace EL
{
  StatusCode AnaFilterHandle ::
  initialize ()
  {
    assert (!m_isInitialized);

    // currently not doing anything here, but presumably if we want to
    // add instrumentation, we may/will need to do some initialization
    // work here.

    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }



  StatusCode AnaFilterHandle ::
  beginExecute () noexcept
  {
    assert (m_isInitialized);
    ANA_MSG_DEBUG ("resetting algorithm-filter-passed flag at beginning of execute()");
    m_setFilterPassed (false);
    return StatusCode::SUCCESS;
  }



  StatusCode AnaFilterHandle ::
  setPassed (bool val_passed) noexcept
  {
    assert (m_isInitialized);
    ANA_MSG_DEBUG ("setting algorithm-filter-passed flag to " << val_passed);
    m_setFilterPassed (val_passed);
    return StatusCode::SUCCESS;
  }
}
