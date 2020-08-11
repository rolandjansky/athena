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
    /// this just returns a dummy reference (for now).

    const EventContext& currentContext ();
  }
}

#endif

#endif
