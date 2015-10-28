/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigPassBitsAuxContainer_v1.cxx 703368 2015-10-27 08:45:13Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/TrigPassBitsAuxContainer_v1.h"

namespace xAOD {

   TrigPassBitsAuxContainer_v1::TrigPassBitsAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( size );
      AUX_VARIABLE( passBits );
      AUX_VARIABLE( containerKey );
      AUX_VARIABLE( containerClid );
   }

} // namespace xAOD
