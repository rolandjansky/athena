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
      static const EventContext result;
      return result;
    }
  }
}

#endif
