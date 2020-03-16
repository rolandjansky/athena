/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AnaAlgorithm/FilterReporterParams.h>

#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/StatusCode.h>
#include <cassert>

//
// method implementations
//

namespace EL
{
  StatusCode FilterReporterParams ::
  initialize ()
  {
    assert (!m_isInitialized);

    // currently not doing anything here, but presumably if we want to
    // add instrumentation, we may/will need to do some initialization
    // work here.

    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }
}
