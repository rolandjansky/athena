/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SysFilterReporterParams.h>

#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/StatusCode.h>

//
// method implementations
//

namespace CP
{
  StatusCode SysFilterReporterParams ::
  initialize ()
  {
    if (m_isInitialized)
    {
      ANA_MSG_FATAL ("calling initialize twice on SysFilterReporterParams, aborting");
      return StatusCode::FAILURE;
    }

    if (m_eventDecisionOutputDecoration.empty())
    {
      ANA_MSG_ERROR ("no event filter decoration name set");
      return StatusCode::FAILURE;
    }

#ifndef XAOD_STANDALONE
    if (!m_cutFlowSvc.empty())
    {
      ANA_CHECK (m_cutFlowSvc.retrieve());
      m_cutID = m_cutFlowSvc->registerFilter (m_cutFlowSvc.parentName(), m_filterDescription + " (nominal only)");
      if (m_cutID == 0)
      {
        ANA_MSG_ERROR ("problem registering myself with cutflow-svc");
        return StatusCode::FAILURE;
      }
    }
#endif

    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }



  StatusCode SysFilterReporterParams ::
  finalize ()
  {
    if (!m_isInitialized)
    {
      ANA_MSG_FATAL ("using unitialized SysFilterReporterParams, aborting");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO ("Events passing selection for at least one systematic: " << m_passedOne << " / " << m_total << " for " << m_filterDescription);
    ATH_MSG_INFO ("Events passing selection for at nominal: " << m_passedNominal << " / " << m_total << " for " << m_filterDescription);
    ATH_MSG_INFO ("Events passing selection for all systematics: " << m_passedAll << " / " << m_total << " for " << m_filterDescription);
    return StatusCode::SUCCESS;
  }
}
