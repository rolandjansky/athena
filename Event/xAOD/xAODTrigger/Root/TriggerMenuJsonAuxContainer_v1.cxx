/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTrigger/versions/TriggerMenuJsonAuxContainer_v1.h"

namespace xAOD {

   TriggerMenuJsonAuxContainer_v1::TriggerMenuJsonAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( key );
      AUX_VARIABLE( name );

      AUX_VARIABLE( payload );
   }

} // namespace xAOD
