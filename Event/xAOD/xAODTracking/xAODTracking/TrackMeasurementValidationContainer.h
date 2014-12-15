/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_TRACKMEASUREMENTVALIDATIONCONTAINER_H
#define XAODTRACKING_TRACKMEASUREMENTVALIDATIONCONTAINER_H
 
// Local include(s):
#include "xAODTracking/TrackMeasurementValidation.h"
#include "xAODTracking/versions/TrackMeasurementValidationContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "TrackMeasurementValidation container version"
   typedef TrackMeasurementValidationContainer_v1 TrackMeasurementValidationContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackMeasurementValidationContainer , 1123888166 , 1 )
 
#endif // XAODTRACKING_TRACKMEASUREMENTVALIDATIONCONTAINER_H
