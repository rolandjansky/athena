/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitAuxContainer_v2.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Header file for class AFPSiHitAuxcontainer_v2
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPSIHITAUXCONTAINER_V2_H
#define XAODFORWARD_VERSIONS_AFPSIHITAUXCONTAINER_V2_H

#include<vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// local includes
#include "xAODForward/versions/AFPSiHit_v2.h"


namespace xAOD {

  /**
   * @brief Class storing information of xAOD::AFPSiHit_v2 objects.
   *
   * This class actually stores information that is accessed by the xAOD::AFPSiHit_v2.
   */
  class AFPSiHitAuxContainer_v2 : public AuxContainerBase {
 
  public:
    AFPSiHitAuxContainer_v2();
 
  private:
    std::vector< float > depositedCharge;
    std::vector< float > timeOverThreshold;
    std::vector< int > stationID;
    std::vector< int > pixelLayerID;
    std::vector< int > pixelRowIDChip;
    std::vector< int > pixelColIDChip;
  }; // class AFPSiHitAuxContainer_v2

  
} // end xAOD namespace

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPSiHitAuxContainer_v2, xAOD::AuxContainerBase );


#endif //  XAODFORWARD_VERSIONS_AFPSIHITAUXCONTAINER_V2_H

