/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_CALOGPUGENERAL_CU_H
#define ISF_FASTCALOGPU_CALOGPUGENERAL_CU_H

#include "Args.h"

namespace CaloGpuGeneral_cu {

  void simulate_hits( float, int, Chain0_Args&, bool );
  void simulate_A_cu( float, int, Chain0_Args&, bool );

} // namespace CaloGpuGeneral_cu
#endif
