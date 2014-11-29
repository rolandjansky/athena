/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigRinger/versions/TrigRingerRingsAuxContainer_v1.h"

namespace xAOD {

   TrigRingerRingsAuxContainer_v1::TrigRingerRingsAuxContainer_v1():AuxContainerBase(){

      AUX_VARIABLE( rings );
      AUX_VARIABLE( RoIword );
  }
} // namespace xAOD
