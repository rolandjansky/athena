/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Local include(s):
#include "xAODForward/versions/AFPToFHitAuxContainer_v1.h"
 
namespace xAOD {
 
  AFPToFHitAuxContainer_v1::AFPToFHitAuxContainer_v1()
    : AuxContainerBase() {   
    AUX_VARIABLE( stationID );
    AUX_VARIABLE( time );
    AUX_VARIABLE( hptdcChannel );
    AUX_VARIABLE( barInTrainID );
    AUX_VARIABLE( trainID );
    AUX_VARIABLE( pulseLength );
    AUX_VARIABLE( hptdcID );
  }
 
} // namespace xAOD
