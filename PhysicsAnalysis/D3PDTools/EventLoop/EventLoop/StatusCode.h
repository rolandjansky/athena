/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack

#ifndef EVENT_LOOP_STATUS_CODE_HH
#define EVENT_LOOP_STATUS_CODE_HH

#include <EventLoop/Global.h>

#include <AsgMessaging/StatusCode.h>

namespace EL
{
  /// \brief StatusCode definition for legacy code
  ///
  /// This typedef is solely here to be picked up by all the code that
  /// follows the old recommendations.  At some point we may deprecate
  /// using this, but certainly not before release 22, as this has
  /// been used in all of our instructions for user code.
  typedef ::StatusCode StatusCode;
}

#endif
