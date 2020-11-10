/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author Tadej Novak


//
// includes
//

#include <AsgMessaging/MessageCheck.h>
#include <AsgMessaging/StatusCode.h>
#include <cassert>

#include <EventBookkeeperTools/FilterReporterParams.h>

//
// method implementations
//

StatusCode FilterReporterParams ::
initialize (bool enabled)
{
  if (m_isInitialized)
  {
    return StatusCode::SUCCESS;
  }

  m_isEnabled = enabled;

  ANA_CHECK (m_eventInfoKey.initialize(m_isEnabled));

#ifndef XAOD_STANDALONE
  if (m_isEnabled && !m_cutFlowSvc.empty())
  {
    ANA_CHECK (m_cutFlowSvc.retrieve());

    m_cutID = m_cutFlowSvc->registerFilter (m_filterKey, m_filterDescription, false);
    if (m_cutID == 0)
    {
      ANA_MSG_ERROR ("problem registering myself with the CutFlowSvc");
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



std::string FilterReporterParams ::
summary ()
{
  if (!m_isInitialized)
  {
    return std::string();
  }

  std::stringstream stream;
  stream << "accepted " << m_passed << " out of " << m_total << " events for filter "
         << m_filterKey << " (" << m_filterDescription << ")";

  return stream.str();
}
