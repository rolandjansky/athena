/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_TRIGGERDECISIONCOMPONENT_H
#define TRIGGERMENUANALYSIS_TRIGGERDECISIONCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include <string>

namespace NtComponent {
  class TriggerDecisionNtComponent: public NtupleComponent {
  public:
    TriggerDecisionNtComponent(NtupleAlgorithm *mainAlg,
			 NtComponentParameters parameters);
    virtual ~TriggerDecisionNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    bool m_useDecision;

    const Trig::ChainGroup* m_allEF;
    const Trig::ChainGroup* m_allL2;
    const Trig::ChainGroup* m_allL1;

    // ntuple variables
    std::vector<std::string> *m_level1TriggerPasses;
    std::vector<std::string> *m_level2TriggerPasses;
    std::vector<std::string> *m_eventFilterTriggerPasses;

  };
}

#endif
