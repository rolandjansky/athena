/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitAuxContainer_v1.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-14
 * 
 * @brief  Header file for class AFPSiHitAuxcontainer_v1
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPSIHITAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPSIHITAUXCONTAINER_V1_H

#include<vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// local includes
#include "xAODForward/versions/AFPSiHit_v1.h"


namespace xAOD {

  /**
   * @brief Class storing information of xAOD::AFPSiHit_v1 objects.
   *
   * This class actually stores information that is accessed by the xAOD::AFPSiHit_v1.
   */
  class AFPSiHitAuxContainer_v1 : public AuxContainerBase {
 
  public:
    AFPSiHitAuxContainer_v1();
 
  private:
    std::vector< float > depositedCharge;
    std::vector< float > timeOverThreshold;
    //    std::vector< float > discriminator;
    std::vector< int > stationID;
    std::vector< int > pixelLayerID;
    std::vector< int > pixelRowIDChip;
    std::vector< int > pixelColIDChip;
    std::vector< std::vector<AFPSiHit_v1::AFPTrackLink_t> > tracksLinks;
  }; // class AFPSiHitAuxContainer_v1

  
} // end xAOD namespace

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPSiHitAuxContainer_v1, xAOD::AuxContainerBase );


#endif //  XAODFORWARD_VERSIONS_AFPSIHITAUXCONTAINER_V1_H

