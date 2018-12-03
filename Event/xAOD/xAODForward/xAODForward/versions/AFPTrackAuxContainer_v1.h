/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrackAuxContainer_v1.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-14
 * 
 * @brief  Header file for class AFPTrackAuxcontainer
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPTRACKAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPTRACKAUXCONTAINER_V1_H

#include<vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// local includes
#include "xAODForward/versions/AFPTrack_v1.h"


namespace xAOD {

  /**
   * @brief Class storing information accessed by xAOD::AFPTrack_v1
   */
  class AFPTrackAuxContainer_v1 : public AuxContainerBase {
 
  public:
    /// Default constructor
    AFPTrackAuxContainer_v1();
 
  private:
    
    std::vector< int > stationID;
    std::vector< float > xLocal;
    std::vector< float > yLocal;
    std::vector< float > zLocal;
    std::vector< float > xSlope;
    std::vector< float > ySlope;
    // float zSlope() const;
    std::vector< int > nHoles;
    std::vector< int > nHits;
    std::vector< std::vector<AFPTrack_v1::AFPHitLink_t> > hits;
    std::vector< float > chi2;
    std::vector< int > algID;
    
  }; // class AFPTrackAuxContainer_v1

  
} // end xAOD namespace

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPTrackAuxContainer_v1, xAOD::AuxContainerBase );


#endif //  XAODFORWARD_VERSIONS_AFPTRACKAUXCONTAINER_V1_H

