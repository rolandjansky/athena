/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitAuxContainer_v1.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-14
 * 
 * @brief  Implementation file for class AFPSiHitAuxContainer_v1
 * 
 */


// Local include(s):
#include "xAODForward/versions/AFPSiHitAuxContainer_v1.h"
 
namespace xAOD {
 
  AFPSiHitAuxContainer_v1::AFPSiHitAuxContainer_v1()
    : AuxContainerBase() {
    AUX_VARIABLE( depositedCharge );
    AUX_VARIABLE( timeOverThreshold );
    //    AUX_VARIABLE( discriminator );
    AUX_VARIABLE( stationID );
    AUX_VARIABLE( pixelLayerID );
    AUX_VARIABLE( pixelRowIDChip );
    AUX_VARIABLE( pixelColIDChip );
    AUX_VARIABLE( tracksLinks );
  }
 
} // namespace xAOD
