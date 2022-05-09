/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOGPU_CALOGPUGENERAL_H
#define ISF_FASTCALOGPU_CALOGPUGENERAL_H

#include <iostream>
#include "Args.h"

#define MIN_GPU_HITS 256

namespace CaloGpuGeneral {

  void* Rand4Hits_init( long long, unsigned short, unsigned long long, bool );
  void  Rand4Hits_finish( void* );

  void simulate_hits( float, int, Chain0_Args&, bool );

} // namespace CaloGpuGeneral
#endif
