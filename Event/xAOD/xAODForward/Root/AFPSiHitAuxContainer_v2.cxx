/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitAuxContainer_v2.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Implementation file for class AFPSiHitAuxContainer_v2
 * 
 */


// Local include(s):
#include "xAODForward/versions/AFPSiHitAuxContainer_v2.h"
 
namespace xAOD {
 
  AFPSiHitAuxContainer_v2::AFPSiHitAuxContainer_v2()
    : AuxContainerBase() {
    AUX_VARIABLE( depositedCharge );
    AUX_VARIABLE( timeOverThreshold );
    AUX_VARIABLE( stationID );
    AUX_VARIABLE( pixelLayerID );
    AUX_VARIABLE( pixelRowIDChip );
    AUX_VARIABLE( pixelColIDChip );
  }
 
} // namespace xAOD
