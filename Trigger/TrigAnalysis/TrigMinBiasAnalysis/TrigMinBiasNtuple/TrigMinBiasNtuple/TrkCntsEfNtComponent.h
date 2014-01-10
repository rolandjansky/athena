/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_TRKCNTSEFNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_TRKCNTSEFNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

namespace NtComponent {
  class TrkCntsEfNtComponent: public NtupleComponent {
  public:
    TrkCntsEfNtComponent(NtupleAlgorithm *mainAlg,
			 NtComponentParameters parameters);
    virtual ~TrkCntsEfNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_trackCountsCollectionKey;
    
    // NTUPLE variables
    unsigned int m_nElements;
    unsigned int m_hZ0Bins;
    float m_hZ0Min, m_hZ0Max;
    unsigned int m_hPtBins;
    float m_hPtMin, m_hPtMax;
    unsigned int m_hEtaBins;
    float m_hEtaMin, m_hEtaMax;
    unsigned int m_hPhiBins;
    float m_hPhiMin, m_hPhiMax;
    std::vector<float>* m_z0_pt_contents;
    std::vector<float>* m_eta_phi_contents;
  };
}

#endif
