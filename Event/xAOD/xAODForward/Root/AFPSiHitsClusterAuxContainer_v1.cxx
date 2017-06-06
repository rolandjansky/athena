/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitsClusterAuxContainer_v1.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-18
 * 
 * @brief  Implementation file for class AFPSiHitsClusterAuxContainer_v1
 * 
 */

// Local include(s):
#include "xAODForward/versions/AFPSiHitsClusterAuxContainer_v1.h"
 
namespace xAOD {
 
  AFPSiHitsClusterAuxContainer_v1::AFPSiHitsClusterAuxContainer_v1()
    : AuxContainerBase() {

    AUX_VARIABLE( stationID );
    AUX_VARIABLE( pixelLayerID );
    AUX_VARIABLE( xLocal );
    AUX_VARIABLE( xLocalErr );
    AUX_VARIABLE( yLocal );
    AUX_VARIABLE( yLocalErr );
    AUX_VARIABLE( zLocal );
    AUX_VARIABLE( zLocalErr );
    AUX_VARIABLE( depositedCharge );
    AUX_VARIABLE( nHits );
    AUX_VARIABLE( hitsLinks );
    AUX_VARIABLE( clusterAlgID );

  }
 
} // namespace xAOD
