/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitsClusterAuxContainer_v1.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-07-15
 * 
 * @brief  Header file for class AFPSiHitsClusterAuxContainer_v1
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPSIHITSCLUSTERAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPSIHITSCLUSTERAUXCONTAINER_V1_H

#include<vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// local includes
#include "xAODForward/versions/AFPSiHitsCluster_v1.h"

namespace xAOD {

  /**
   * @brief Class storing information of xAOD::AFPSiHitsCluster objects.
   *
   * This class actually stores information that is accessed by the xAOD::AFPSiHitsCluster_v1.
   */
  class AFPSiHitsClusterAuxContainer_v1 : public AuxContainerBase {
 
  public:
    AFPSiHitsClusterAuxContainer_v1();
 
  private:
    std::vector< int > stationID;
    std::vector< int > pixelLayerID;
    std::vector< float > xLocal;
    std::vector< float > xLocalErr;
    std::vector< float > yLocal;
    std::vector< float > yLocalErr;
    std::vector< float > zLocal;
    std::vector< float > zLocalErr;
    std::vector< float > depositedCharge;
    std::vector< int > nHits;
    std::vector< std::vector<AFPSiHitsCluster_v1::AFPHitLink_t> > hitsLinks;
    std::vector< int > clusterAlgID;

  }; // class AFPSiHitsClusterAuxContainer_v1

  
} // end xAOD namespace

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPSiHitsClusterAuxContainer_v1, xAOD::AuxContainerBase );


#endif //  XAODFORWARD_VERSIONS_AFPSIHITSCLUSTERAUXCONTAINER_V1_H

