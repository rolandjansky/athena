/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_TRACKSTATEVALIDATIONAUXCONTAINER_H
#define XAODTRACKING_TRACKSTATEVALIDATIONAUXCONTAINER_H
 
// Local include(s):
#include "xAODTracking/versions/TrackStateValidationAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current TrackStateValidation auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::TrackStateValidationContainer,
   /// so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef TrackStateValidationAuxContainer_v1 TrackStateValidationAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackStateValidationAuxContainer , 1220703896 , 1 )

#endif // XAODTRACKING_TRACKSTATEVALIDATIONAUXCONTAINER_H
