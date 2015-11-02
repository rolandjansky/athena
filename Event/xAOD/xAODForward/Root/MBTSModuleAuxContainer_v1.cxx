/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODForward/versions/MBTSModuleAuxContainer_v1.h"

namespace xAOD {

   MBTSModuleAuxContainer_v1::MBTSModuleAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( e );
      AUX_VARIABLE( eta  );
      AUX_VARIABLE( phi );
      AUX_VARIABLE( time );
      AUX_VARIABLE( mbtsQuality );
      AUX_VARIABLE( type );
      AUX_VARIABLE( mbtsModule );
      AUX_VARIABLE( channel );
   }

} // namespace xAOD
