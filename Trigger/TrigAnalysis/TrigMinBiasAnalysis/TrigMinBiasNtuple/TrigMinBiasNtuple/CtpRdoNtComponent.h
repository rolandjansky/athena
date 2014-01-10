/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_CTPRDONTCOMPONENT_H
#define TRIGMINBIASNTUPLE_CTPRDONTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

namespace NtComponent {
  class CtpRdoNtComponent: public NtupleComponent {
  public:
    CtpRdoNtComponent(NtupleAlgorithm *mainAlg,
		      NtComponentParameters parameters);
    virtual ~CtpRdoNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_ctp_RdoKey;

    // NTUPLE variables
    uint32_t m_nBC;
    uint32_t m_L1A_BC;
    uint32_t m_timeSec; 
    uint32_t m_timeNanoSec;
    std::vector<uint32_t>* m_pit;
    std::vector<uint32_t>* m_tbp;
    std::vector<uint32_t>* m_tap;
    std::vector<uint32_t>* m_tav;
  };
}

#endif
