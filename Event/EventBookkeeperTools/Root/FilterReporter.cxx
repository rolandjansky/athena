/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author Tadej Novak


//
// includes
//

#include <AsgDataHandles/ReadHandle.h>
#include <AsgMessaging/MessageCheck.h>
#include <AsgTools/CurrentContext.h>
#include <cassert>

#include <EventBookkeeperTools/FilterReporter.h>
#include <EventBookkeeperTools/FilterReporterParams.h>

//
// method implementations
//

FilterReporter ::
FilterReporter (FilterReporterParams& val_params,
                bool val_passedDefault)
  : m_params (val_params)
  , m_passed (val_passedDefault)
  , m_eventContext (&Gaudi::Hive::currentContext())
{
  if (!m_params.m_isInitialized)
  {
    throw std::logic_error ("using uninitialized FilterReporterParams");
  }
}



FilterReporter ::
FilterReporter (const FilterReporterParams& val_params,
                bool val_passedDefault,
                const EventContext& val_eventContext)
  : m_params (val_params)
  , m_passed (val_passedDefault)
  , m_eventContext (&val_eventContext)
{
  if (!m_params.m_isInitialized)
  {
    throw std::logic_error ("using uninitialized FilterReporterParams");
  }
}



FilterReporter ::
~FilterReporter () noexcept
{
  m_params.m_setFilterPassed (m_passed, m_eventContext);

  if (m_passed)
    m_params.m_passed += 1;
  m_params.m_total += 1;

#ifndef XAOD_STANDALONE
  if (m_passed && m_params.m_cutID != 0)
  {
    SG::ReadHandle<xAOD::EventInfo> evtInfo (m_params.m_eventInfoKey, *m_eventContext);
    // Only try to access the mcEventWeight if we are running on Monte Carlo
    if (evtInfo.isValid() && evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION)) {
      m_params.m_cutFlowSvc->addEvent (m_params.m_cutID, evtInfo->mcEventWeights());
    } else {
      m_params.m_cutFlowSvc->addEvent (m_params.m_cutID, 1.0);
    }
  }
#endif
}



void FilterReporter ::
setPassed (bool val_passed) noexcept
{
  m_passed = val_passed;
}
