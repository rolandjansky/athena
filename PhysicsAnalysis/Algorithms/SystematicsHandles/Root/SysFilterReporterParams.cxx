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

    ATH_MSG_INFO ("Events passing selection for at least one systematic: " << m_passedOne << " / " << m_total);
    ATH_MSG_INFO ("Events passing selection for all systematics: " << m_passedAll << " / " << m_total);
    return StatusCode::SUCCESS;
  }
}
