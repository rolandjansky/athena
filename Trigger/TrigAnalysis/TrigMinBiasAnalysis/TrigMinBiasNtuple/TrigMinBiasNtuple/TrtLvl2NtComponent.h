/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_TRTLVL2NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_TRTLVL2NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

namespace NtComponent {
  class TrtLvl2NtComponent: public NtupleComponent {
  public:
    TrtLvl2NtComponent(NtupleAlgorithm *mainAlg,
		       NtComponentParameters parameters);
    virtual ~TrtLvl2NtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_trigTrtHitCountsCollectionKey;

    // NTUPLE variables
    unsigned int m_nElements;
    unsigned int m_hTotBins;
    float m_hTotMin;
    float m_hTotMax;
    std::vector<float>* m_endcapC_contents;
    std::vector<float>* m_barrel_contents;
    std::vector<float>* m_endcapA_contents;
  };
}

#endif
