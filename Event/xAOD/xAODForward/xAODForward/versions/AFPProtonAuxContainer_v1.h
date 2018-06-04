/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPProtonAuxContainer_v1.h
 * @author Krzysztof Cieśla <krzysztof.marcin.ciesla@cern.ch>
 * @date   2018-04-05
 * 
 * @brief  Header file for class AFPProtonAuxcontainer_v1
 * 
 */

#ifndef XAODFORWARD_VERSIONS_AFPPROTONAUXCONTAINER_V1_H
#define XAODFORWARD_VERSIONS_AFPPROTONAUXCONTAINER_V1_H

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "xAODForward/versions/AFPProton_v1.h"

namespace xAOD {

  class AFPProtonAuxContainer_v1 : public AuxContainerBase
  {
  public:
    AFPProtonAuxContainer_v1();

  private:
    std::vector<float>  px;
    std::vector<float>  py;
    std::vector<float>  pz;
    std::vector<float>  e;
  
    std::vector<float>  chi2;
    std::vector<int>    side;
    std::vector<int>    methodID;

    std::vector<std::vector<AFPProton_v1::AFPTrackLink_t> > afpTrackLinks;
  }; // class AFPProtonAuxContainer_v1
    
} // namespace xAOD

// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::AFPProtonAuxContainer_v1, xAOD::AuxContainerBase ); 

#endif
