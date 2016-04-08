/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v2.h"

namespace xAOD {

   TrigRingerRingsAuxContainer_v2::TrigRingerRingsAuxContainer_v2():AuxContainerBase(){

      AUX_VARIABLE( rings );
      AUX_VARIABLE( emClusterLink );
      
  }
} // namespace xAOD
