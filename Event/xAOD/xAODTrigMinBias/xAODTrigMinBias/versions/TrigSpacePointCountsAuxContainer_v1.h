/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigSpacePointCountsAUXCONTAINTER_V1_H
#define xAODTrigMinBias_TrigSpacePointCountsAUXCONTAINTER_V1_H

#include "xAODCore/AuxContainerBase.h"

#include <vector>

namespace xAOD {

  class TrigSpacePointCountsAuxContainer_v1 : public AuxContainerBase {

  public:
    TrigSpacePointCountsAuxContainer_v1();

  private:
    std::vector< std::vector<float> > contentsPixelClusEndcapC;
    std::vector< std::vector<float> > contentsPixelClusBarrel;
    std::vector< std::vector<float> > contentsPixelClusEndcapA;
    std::vector<unsigned int>  pixelClusTotBins;
    std::vector<float> pixelClusTotMin;
    std::vector<float> pixelClusTotMax;
    std::vector<unsigned int>  pixelClusSizeBins;
    std::vector<float> pixelClusSizeMin;
    std::vector<float> pixelClusSizeMax;
    std::vector<unsigned int> sctSpEndcapC;
    std::vector<unsigned int> sctSpBarrel;
    std::vector<unsigned int> sctSpEndcapA;
  };

}

#include "xAODCore/BaseInfo.h"
SG_BASE(   xAOD::TrigSpacePointCountsAuxContainer_v1, xAOD::AuxContainerBase );

#endif
