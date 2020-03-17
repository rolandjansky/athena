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
  FilterReporter (FilterReporterParams& val_handle,
                  bool val_passedDefault)
    : AsgMessagingForward (&val_handle)
    , m_handle (val_handle)
    , m_passed (val_passedDefault)
  {
    assert (m_handle.m_isInitialized);
  }



  FilterReporter ::
  FilterReporter (const FilterReporterParams& val_handle,
                  bool val_passedDefault,
                  const EventContext& /*val_eventContext*/)
    : AsgMessagingForward (&val_handle)
    , m_handle (val_handle)
    , m_passed (val_passedDefault)
  {
    assert (m_handle.m_isInitialized);
  }



  FilterReporter ::
  ~FilterReporter () noexcept
  {
    ANA_MSG_DEBUG ("setting algorithm-filter-passed flag to " << m_passed);
    m_handle.m_setFilterPassed (m_passed);
  }



  void FilterReporter ::
  setPassed (bool val_passed) noexcept
  {
    ANA_MSG_DEBUG ("change algorithm-filter-passed flag from " << m_passed << " to " << val_passed);
    m_passed = val_passed;
  }
}
