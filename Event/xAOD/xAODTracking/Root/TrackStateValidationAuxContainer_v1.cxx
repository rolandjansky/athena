/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTracking/versions/TrackStateValidationAuxContainer_v1.h"
 
namespace xAOD {
 
  TrackStateValidationAuxContainer_v1::TrackStateValidationAuxContainer_v1()
    : AuxContainerBase() {
    
    AUX_VARIABLE( type );
    AUX_VARIABLE( detElementId );
    AUX_VARIABLE( detType );
    
    AUX_VARIABLE( localX );
    AUX_VARIABLE( localY );
    AUX_VARIABLE( localTheta );
    AUX_VARIABLE( localPhi );
    AUX_VARIABLE( unbiasedResidualX );
    AUX_VARIABLE( unbiasedResidualY );
    AUX_VARIABLE( unbiasedPullX );
    AUX_VARIABLE( unbiasedPullY );
    AUX_VARIABLE( biasedResidualX );
    AUX_VARIABLE( biasedResidualY );
    AUX_VARIABLE( biasedPullX );
    AUX_VARIABLE( biasedPullY );

    AUX_VARIABLE( trackMeasurementValidationLink );

  }

} // namespace xAOD
