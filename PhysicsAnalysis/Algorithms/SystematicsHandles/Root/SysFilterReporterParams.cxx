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
#include <cassert>

//
// method implementations
//

namespace CP
{
  StatusCode SysFilterReporterParams ::
  initialize ()
  {
    assert (!m_isInitialized);

    if (m_eventDecisionOutputDecoration.empty()) {
        ANA_MSG_ERROR("no event filter decoration name set");
        return StatusCode::FAILURE;
    }

    m_isInitialized = true;
    return StatusCode::SUCCESS;
  }

  StatusCode SysFilterReporterParams ::
  finalize ()
  {
    ATH_MSG_INFO ("Events passing selection for at least one systematic: " << m_passedOne << " / " << m_total);
    ATH_MSG_INFO ("Events passing selection for all systematics: " << m_passedAll << " / " << m_total);
    return StatusCode::SUCCESS;
  }
}
