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
    if (m_isInitialized)
    {
      ANA_MSG_FATAL ("initializing filter reporter twice, aborting");
      return StatusCode::FAILURE;
    }

#ifndef XAOD_STANDALONE
    if (!m_cutFlowSvc.empty())
    {
      ANA_CHECK (m_cutFlowSvc.retrieve());
      m_cutID = m_cutFlowSvc->registerFilter (m_cutFlowSvc.parentName(), m_filterDescription);
      if (m_cutID == 0)
      {
        ANA_MSG_ERROR ("problem registering myself with cutflow-svc");
        return StatusCode::FAILURE;
      }
    }
#endif

    // currently not doing anything here, but presumably if we want to
    // add instrumentation, we may/will need to do some initialization
    // work here.

    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }



  StatusCode FilterReporterParams ::
  finalize ()
  {
    if (!m_isInitialized)
    {
      ANA_MSG_FATAL ("using uninitializing filter reporter, aborting");
      return StatusCode::FAILURE;
    }

    ANA_MSG_INFO ("accepted " << m_passed << " out of " << m_total << " events for filter " << m_filterDescription);

    return StatusCode::SUCCESS;
  }
}
