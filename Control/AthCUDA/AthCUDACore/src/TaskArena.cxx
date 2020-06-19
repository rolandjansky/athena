//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "AthCUDACore/TaskArena.h"

namespace AthCUDA {

   tbb::task_arena& taskArena() {

      static tbb::task_arena arena( 1, 0 );
      return arena;
   }

} // namespace AthCUDA
