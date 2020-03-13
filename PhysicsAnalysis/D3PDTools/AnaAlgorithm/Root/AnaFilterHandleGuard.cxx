/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


//
// includes
//

#include <AnaAlgorithm/AnaFilterHandleGuard.h>

#include <AnaAlgorithm/AnaFilterHandle.h>
#include <AsgMessaging/MessageCheck.h>
#include <cassert>

//
// method implementations
//

namespace EL
{
  AnaFilterHandleGuard ::
  AnaFilterHandleGuard (AnaFilterHandle& val_handle,
                        bool val_passedDefault)
    : AsgMessagingForward (&val_handle)
    , m_handle (val_handle)
    , m_passed (val_passedDefault)
  {
    assert (m_handle.m_isInitialized);
  }



  AnaFilterHandleGuard ::
  AnaFilterHandleGuard (const AnaFilterHandle& val_handle,
                        const EventContext& /*val_eventContext*/,
                        bool val_passedDefault)
    : AsgMessagingForward (&val_handle)
    , m_handle (val_handle)
    , m_passed (val_passedDefault)
  {
    assert (m_handle.m_isInitialized);
  }



  AnaFilterHandleGuard ::
  ~AnaFilterHandleGuard () noexcept
  {
    ANA_MSG_DEBUG ("setting algorithm-filter-passed flag to " << m_passed);
    m_handle.m_setFilterPassed (m_passed);
  }



  void AnaFilterHandleGuard ::
  setPassed (bool val_passed) noexcept
  {
    ANA_MSG_DEBUG ("change algorithm-filter-passed flag from " << m_passed << " to " << val_passed);
    m_passed = val_passed;
  }
}
