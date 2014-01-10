/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_CTPDECISIONNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_CTPDECISIONNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include <string>

class CTP_Decision;

namespace NtComponent {
  class CtpDecisionNtComponent: public NtupleComponent {
  public:
    CtpDecisionNtComponent(NtupleAlgorithm *mainAlg,
			   NtComponentParameters parameters);
    virtual ~CtpDecisionNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_ctp_DecisionKey;

    // NTUPLE variables
    std::vector<std::string>* m_ctpDecisionItems;
    std::vector<uint32_t>* m_ctpDecisionWords;
    unsigned int m_ctpDecisionTypeWord;
  };
}

#endif
