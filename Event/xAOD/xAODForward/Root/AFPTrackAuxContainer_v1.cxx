/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrackAuxContainer_v1.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-14
 * 
 * @brief  Implementation file for class xAOD::AFPTrackAuxContainer_v1
 * 
 */


// Local include(s):
#include "xAODForward/versions/AFPTrackAuxContainer_v1.h"
 
namespace xAOD {
 
  AFPTrackAuxContainer_v1::AFPTrackAuxContainer_v1()
    : AuxContainerBase() {
    
  AUX_VARIABLE( stationID );
  AUX_VARIABLE( xLocal );
  AUX_VARIABLE( yLocal );
  AUX_VARIABLE( zLocal );
  AUX_VARIABLE( xSlope );
  AUX_VARIABLE( ySlope );
  //  AUX_VARIABLE( float zSlope() const );
  AUX_VARIABLE( nHoles );
  AUX_VARIABLE( nHits );
  AUX_VARIABLE( chi2 );
  AUX_VARIABLE( algID );
  AUX_VARIABLE( hits );
}
 
} // namespace xAOD
