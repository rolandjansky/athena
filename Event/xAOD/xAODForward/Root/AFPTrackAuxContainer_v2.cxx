/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrackAuxContainer_v2.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Implementation file for class xAOD::AFPTrackAuxContainer_v2
 * 
 */


// Local include(s):
#include "xAODForward/versions/AFPTrackAuxContainer_v2.h"
 
namespace xAOD {
 
  AFPTrackAuxContainer_v2::AFPTrackAuxContainer_v2()
    : AuxContainerBase() {
    
    AUX_VARIABLE( stationID );
    AUX_VARIABLE( xLocal );
    AUX_VARIABLE( yLocal );
    AUX_VARIABLE( zLocal );
    AUX_VARIABLE( xSlope );
    AUX_VARIABLE( ySlope );
    AUX_VARIABLE( nHoles );
    AUX_VARIABLE( nClusters );
    AUX_VARIABLE( chi2 );
    AUX_VARIABLE( algID );
    AUX_VARIABLE( clusters );
  }
 
} // namespace xAOD
