/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigTrackCountsAUXCONTAINTER_V1_H
#define xAODTrigMinBias_TrigTrackCountsAUXCONTAINTER_V1_H

#include "xAODCore/AuxContainerBase.h"

#include <vector>

namespace xAOD {

  class TrigTrackCountsAuxContainer_v1 : public AuxContainerBase {

  public:
    TrigTrackCountsAuxContainer_v1();

  private:
    std::vector< std::vector<float> > z0_pt;
    std::vector< std::vector<float> > eta_phi;
    
    std::vector<unsigned int>  z0Bins;
    std::vector<float> z0Min;
    std::vector<float> z0Max;
    
    std::vector<unsigned int>  ptBins;
    std::vector<float> ptMin;
    std::vector<float> ptMax;
    
    std::vector<unsigned int>  etaBins;
    std::vector<float> etaMin;
    std::vector<float> etaMax;
    
    std::vector<unsigned int>  phiBins;
    std::vector<float> phiMin;
    std::vector<float> phiMax;

  };

}

#include "xAODCore/BaseInfo.h"
SG_BASE(   xAOD::TrigTrackCountsAuxContainer_v1, xAOD::AuxContainerBase );

#endif
