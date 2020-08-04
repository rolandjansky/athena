/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AnaAlgorithm/FilterReporter.h>

#include <AnaAlgorithm/FilterReporterParams.h>
#include <AsgMessaging/MessageCheck.h>
#include <cassert>

//
// method implementations
//

namespace EL
{
  FilterReporter ::
  FilterReporter (FilterReporterParams& val_params,
                  bool val_passedDefault)
    : AsgMessagingForward (&val_params)
    , m_params (val_params)
    , m_passed (val_passedDefault)
  {
    assert (m_params.m_isInitialized);
  }



  FilterReporter ::
  FilterReporter (const FilterReporterParams& val_params,
                  bool val_passedDefault,
                  const EventContext& /*val_eventContext*/)
    : AsgMessagingForward (&val_params)
    , m_params (val_params)
    , m_passed (val_passedDefault)
  {
    if (!m_params.m_isInitialized)
    {
      ANA_MSG_FATAL ("using uninitialized FilterReporterParams, throwing exception");
      throw std::logic_error ("using uninitialized FilterReporterParams");
    }
  }



  FilterReporter ::
  ~FilterReporter () noexcept
  {
    ANA_MSG_DEBUG ("setting algorithm-filter-passed flag to " << m_passed);
    m_params.m_setFilterPassed (m_passed);
    if (m_passed)
      m_params.m_passed += 1;
    m_params.m_total += 1;

#ifndef XAOD_STANDALONE
    if (m_passed && m_params.m_cutID != 0)
      m_params.m_cutFlowSvc->addEvent (m_params.m_cutID);
#endif
  }



  void FilterReporter ::
  setPassed (bool val_passed) noexcept
  {
    ANA_MSG_DEBUG ("change algorithm-filter-passed flag from " << m_passed << " to " << val_passed);
    m_passed = val_passed;
  }
}
