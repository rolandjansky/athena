/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_TOOLS__CURRENT_CONTEXT_H
#define ASG_TOOLS__CURRENT_CONTEXT_H

#ifndef XAOD_STANDALONE
#include <GaudiKernel/ThreadLocalContext.h>
#else

class EventContext;

namespace Gaudi
{
  namespace Hive
  {
    /// \brief the current EventContext
    ///
    /// This is mostly meant so that you can call a function requiring
    /// an `EventContext` even if you yourself got called without one.
    /// This matches the signature from GaudiHive, but in AnalysisBase
    /// this just returns a reference to a dummy object (for now).

    const EventContext& currentContext ();
  }
}

/// \warn this is a dummy class, do not try to use it for anything,
/// except passing arround a reference to it where needed.
///
/// This may be turned into a more meaningful class at some point, but
/// for now, just get a reference to an `EventContext` object via
/// `Gaudi::Hive::currentContext()`.

class EventContext final
{
  // deleting these for now, as users should be passing these objects
  // by reference.
  EventContext (const EventContext&) noexcept = delete;
  EventContext& operator = (const EventContext&) noexcept = delete;

  // making this private, so that users can't instantiate their own
  // EventContext.
  EventContext () noexcept = default;
  friend const EventContext& Gaudi::Hive::currentContext ();
};

#endif

#endif
