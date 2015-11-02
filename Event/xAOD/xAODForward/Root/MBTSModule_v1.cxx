/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/MBTSModule_v1.h"

namespace xAOD {

   MBTSModule_v1::MBTSModule_v1()
      : SG::AuxElement() {

   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MBTSModule_v1, float, e , setE )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MBTSModule_v1, float, eta , setEta ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MBTSModule_v1, float, phi , setPhi )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MBTSModule_v1, float, time , setTime )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MBTSModule_v1, int, mbtsQuality, setMbtsQuality )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MBTSModule_v1, int, mbtsModule, setMbtsModule )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MBTSModule_v1, int, type, setType )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( MBTSModule_v1, int, channel, setChannel )

} // namespace xAOD
