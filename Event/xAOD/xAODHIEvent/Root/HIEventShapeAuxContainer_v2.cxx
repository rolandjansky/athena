/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShapeAuxContainer_v2.cxx 693494 2015-09-07 14:59:45Z krasznaa $

// Local include(s):
#include "xAODHIEvent/versions/HIEventShapeAuxContainer_v2.h"

namespace xAOD {

   HIEventShapeAuxContainer_v2::HIEventShapeAuxContainer_v2()
      : AuxContainerBase() {

      AUX_VARIABLE( et );
      AUX_VARIABLE( area );
      AUX_VARIABLE( rho );
      AUX_VARIABLE( nCells );

      AUX_VARIABLE( etCos ); 
      AUX_VARIABLE( etSin );
      
      AUX_VARIABLE( etaMin );
      AUX_VARIABLE( etaMax );
      AUX_VARIABLE( layer );
   }

} // namespace xAOD
