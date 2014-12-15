/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_TRACKMEASUREMENTVALIDATIONAUXCONTAINER_H
#define XAODTRACKING_TRACKMEASUREMENTVALIDATIONAUXCONTAINER_H
 
// Local include(s):
#include "xAODTracking/versions/TrackMeasurementValidationAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current Muon auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::TrackMeasurementValidationContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef TrackMeasurementValidationAuxContainer_v1 TrackMeasurementValidationAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackMeasurementValidationAuxContainer , 1311112289 , 1 )
 
#endif // XAODTRACKING_TRACKMEASUREMENTVALIDATIONAUXCONTAINER_H
