/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



//
// includes
//

#include <AsgTools/CurrentContext.h>

#ifdef XAOD_STANDALONE

//
// method implementations
//

namespace Gaudi
{
  namespace Hive
  {
    const EventContext& currentContext ()
    {
      // For now there is no actual EventContext class in AnalysisBase,
      // so for now we return a null-reference.  Since there is
      // currently no (legal) way to dereference an EventContext in
      // AnalysisBase, so this is fine, and indeed should catch any case
      // in which the user might somehow access this anyways.
      static EventContext *result {nullptr};
      return *result;
    }
  }
}

#endif
