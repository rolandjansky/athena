/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPVertexAuxContainer_v1.h
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-12-08
 * 
 * @brief  Header file for class AFPVertexAuxcontainer
 * 
 */


#ifndef XAODFORWARD_VERSIONS_AFPVERTEXAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPVERTEXAUXCONTAINER_V1_H

#include<vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// local includes
#include "xAODForward/versions/AFPVertex_v1.h"

namespace xAOD {

  /**
   * @brief Class storing information accessed by xAOD::AFPVertex_v1
   */

  class AFPVertexAuxContainer_v1 : public AuxContainerBase {
 
  public:
    /// Default constructor
    AFPVertexAuxContainer_v1();
 
  private:
    
    std::vector< float > position;
    std::vector< std::vector<AFPVertex_v1::AFPProtonLink_t> > protons;
    std::vector< std::vector<AFPVertex_v1::AFPToFTrackLink_t> > tofTracks;
    std::vector< float > distA;
    std::vector< float > distC;
    std::vector< int > algID;
    
  }; // class AFPVertexAuxContainer_v1

  
} // end xAOD namespace

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPVertexAuxContainer_v1, xAOD::AuxContainerBase );


#endif //  XAODFORWARD_VERSIONS_AFPVERTEXAUXCONTAINER_V1_H

