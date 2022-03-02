/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPToFTrackAuxContainer_v1.h
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-11-11
 * 
 * @brief  Header file for class AFPToFTrackAuxcontainer
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPTOFTRACKAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPTOFTRACKAUXCONTAINER_V1_H

#include<vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// local includes
#include "xAODForward/versions/AFPToFTrack_v1.h"


namespace xAOD {

  /**
   * @brief Class storing information accessed by xAOD::AFPToFTrack_v1
   */
  class AFPToFTrackAuxContainer_v1 : public AuxContainerBase {
 
  public:
    /// Default constructor
    AFPToFTrackAuxContainer_v1();
 
  private:
    
    std::vector< int > stationID;
    std::vector< int > trainID;
    std::vector< float > trainTime;
    std::vector< int > trainSize;
    std::vector< int > trainNSat;
    std::vector< std::vector<AFPToFTrack_v1::AFPToFHitLink_t> > bars;
    std::vector< int > algID;
    
  }; // class AFPToFTrackAuxContainer_v1

  
} // end xAOD namespace

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPToFTrackAuxContainer_v1, xAOD::AuxContainerBase );


#endif //  XAODFORWARD_VERSIONS_AFPTOFTRACKAUXCONTAINER_V1_H

