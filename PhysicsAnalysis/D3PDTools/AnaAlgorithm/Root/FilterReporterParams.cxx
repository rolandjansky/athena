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

    // this is directly lifted from \ref AthFilterAlgorithm, not sure
    // if we should be this fault-tolerant there, i.e. should we just
    // fail if the cut-flow service can't be found or configured?
    // after all the user could always configure not to use the
    // `CutFlowSvc`.
#ifndef XAOD_STANDALONE
    if (m_cutFlowSvc.empty())
    {
      if (m_cutFlowSvc.retrieve().isSuccess())
      {
        m_cutID = m_cutFlowSvc->registerFilter (m_algName, m_filterDescription);
        if (m_cutID == 0)
        {
          ATH_MSG_INFO("problem registering myself with cutflow-svc");
        } else
        {
          ATH_MSG_VERBOSE("registered with cutflow-svc");
        }
      } else
      {
        ANA_MSG_INFO ("couldn't retrieve cut-flow service, not reporting cut-flows");
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
