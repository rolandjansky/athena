/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrackAuxContainer_v2.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Header file for class AFPTrackAuxcontainer
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPTRACKAUXCONTAINER_V2_H
#define XAODFORWARD_VERSIONS_AFPTRACKAUXCONTAINER_V2_H

#include<vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// local includes
#include "xAODForward/versions/AFPTrack_v2.h"


namespace xAOD {

  /**
   * @brief Class storing information accessed by xAOD::AFPTrack_v2
   */
  class AFPTrackAuxContainer_v2 : public AuxContainerBase {
 
  public:
    /// Default constructor
    AFPTrackAuxContainer_v2();
 
  private:
    
    std::vector< int > stationID;
    std::vector< float > xLocal;
    std::vector< float > yLocal;
    std::vector< float > zLocal;
    std::vector< float > xSlope;
    std::vector< float > ySlope;
    std::vector< int > nHoles;
    std::vector< int > nClusters;
    std::vector< std::vector<AFPTrack_v2::AFPClusterLink_t> > clusters;
    std::vector< float > chi2;
    std::vector< int > algID;
    
  }; // class AFPTrackAuxContainer_v2

  
} // end xAOD namespace

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPTrackAuxContainer_v2, xAOD::AuxContainerBase );


#endif //  XAODFORWARD_VERSIONS_AFPTRACKAUXCONTAINER_V2_H

