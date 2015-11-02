/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/ForwardEventInfo_v1.h"

namespace xAOD {

   ForwardEventInfo_v1::ForwardEventInfo_v1()
      : SG::AuxElement() {

   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( ForwardEventInfo_v1, float, timeDiff , setTimeDiff )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( ForwardEventInfo_v1, float, timeA , setTimeA ) 
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( ForwardEventInfo_v1, float, timeC , setTimeC )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( ForwardEventInfo_v1, unsigned short, countA , setCountA )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( ForwardEventInfo_v1, unsigned short, countC , setCountC )

} // namespace xAOD
