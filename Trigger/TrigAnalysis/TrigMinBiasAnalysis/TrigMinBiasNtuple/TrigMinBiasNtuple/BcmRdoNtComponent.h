/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_BCMRDONTCOMPONENT_H
#define TRIGMINBIASNTUPLE_BCMRDONTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

namespace NtComponent {
  class BcmRdoNtComponent: public NtupleComponent {
  public:
    BcmRdoNtComponent(NtupleAlgorithm *mainAlg,
		      NtComponentParameters parameters);
    virtual ~BcmRdoNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_bcmRdoContainerKey;

    static const int s_max_hits = 10;
    static const int s_max_bcm = 8;

    // NTUPLE variables
    int m_bcm_nbcm;
    std::vector<int>* m_bcm_channel;
    std::vector<int>* m_bcm_nhits;
    std::vector<std::vector<int> >* m_bcm_pulse1pos;
    std::vector<std::vector<int> >* m_bcm_pulse1width;
    std::vector<std::vector<int> >* m_bcm_pulse2pos;
    std::vector<std::vector<int> >* m_bcm_pulse2width;
    std::vector<std::vector<int> >* m_bcm_lvl1a;
    std::vector<std::vector<int> >* m_bcm_bcid;
    std::vector<std::vector<int> >* m_bcm_lvl1id;
  };
}

#endif
