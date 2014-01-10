/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_SPLVL2NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_SPLVL2NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

namespace NtComponent {
  class SpLvl2NtComponent: public NtupleComponent {
  public:
    SpLvl2NtComponent(NtupleAlgorithm *mainAlg,
		      NtComponentParameters parameters);
    virtual ~SpLvl2NtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_spacePointLvl2ContainerName;

    // NTUPLE variables
    unsigned int m_nElements;
    unsigned int m_hPixelClusTotBins;
    float m_hPixelClusTotMin;
    float m_hPixelClusTotMax;
    unsigned int m_hPixelClusSizeBins;
    float m_hPixelClusSizeMin;
    float m_hPixelClusSizeMax;
    std::vector<float>* m_pixelClusEndcapC_contents;
    std::vector<float>* m_pixelClusBarrel_contents;
    std::vector<float>* m_pixelClusEndcapA_contents;
    std::vector<unsigned int>* m_droppedPixelModuleIds;
    unsigned int m_sctSpEndcapC;
    unsigned int m_sctSpBarrel;
    unsigned int m_sctSpEndcapA;
    std::vector<unsigned int>* m_droppedSctModuleIds;
  };
}

#endif
