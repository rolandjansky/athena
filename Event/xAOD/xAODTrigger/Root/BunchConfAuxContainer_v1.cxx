/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfAuxContainer_v1.cxx 583374 2014-02-14 15:52:54Z krasznaa $

// Local include(s):
#include "xAODTrigger/versions/BunchConfAuxContainer_v1.h"

namespace xAOD {

   BunchConfAuxContainer_v1::BunchConfAuxContainer_v1()
      : AuxContainerBase() {

      AUX_VARIABLE( configID );

      AUX_VARIABLE( collidingBCIDs );
      AUX_VARIABLE( intensitiesBeam1 );
      AUX_VARIABLE( intensitiesBeam2 );

      AUX_VARIABLE( unpairedBCIDsBeam1 );
      AUX_VARIABLE( unpairedBCIDsBeam2 );
      AUX_VARIABLE( unpairedIntensisitesBeam1 );
      AUX_VARIABLE( unpairedIntensisitesBeam2 );
   }

} // namespace xAOD
