/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <SystematicsHandles/SysFilterReporter.h>

#include <SystematicsHandles/SysFilterReporterCombiner.h>
#include <SystematicsHandles/SysFilterReporterParams.h>
#include <AsgMessaging/MessageCheck.h>

//
// method implementations
//

namespace CP
{
  SysFilterReporter ::
  SysFilterReporter (SysFilterReporterCombiner& val_combiner,
                     const SystematicSet& val_sys)
    : AsgMessagingForward (&val_combiner)
    , m_combiner (val_combiner)
    , m_sys (val_sys)
    , m_passed (val_combiner.m_passedDefault)
  {
    ANA_CHECK_THROW (val_combiner.m_params.m_eventInfoHandle.getCopy (m_eventInfo, m_sys));
  }



  SysFilterReporter ::
  ~SysFilterReporter () noexcept
  {
    ANA_MSG_DEBUG ("setting systematics-filter-passed flag to " << m_passed << " for " << m_sys.name());
    if (m_passed)
      m_combiner.m_passedOne = true;
    else
      m_combiner.m_passedAll = false;
    // decorate event info
    m_combiner.m_params.m_eventDecisionOutputDecoration.set (*m_eventInfo, m_passed, m_sys);

    // only recording nominal event selection for now
    if (m_passed && m_sys.empty())
      m_combiner.m_params.m_passedNominal += 1;

#ifndef XAOD_STANDALONE
    // only recording nominal event selection for now
    if (m_passed && m_sys.empty() && m_combiner.m_params.m_cutID != 0)
      m_combiner.m_params.m_cutFlowSvc->addEvent (m_combiner.m_params.m_cutID);
#endif
  }



  void SysFilterReporter ::
  setPassed (bool val_passed) noexcept
  {
    ANA_MSG_DEBUG ("change systematics-filter-passed flag to " << m_passed << " for " << m_sys.name());
    m_passed = val_passed;
  }
}
