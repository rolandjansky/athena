/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_ZDCDIGITSNTCOMPONENT_H
#define TRIGMINBIASNTUPLE_ZDCDIGITSNTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>
#include <string>

namespace NtComponent {
  class ZdcDigitsNtComponent: public NtupleComponent {
  public:
    ZdcDigitsNtComponent(NtupleAlgorithm *mainAlg,
			 NtComponentParameters parameters);
    virtual ~ZdcDigitsNtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_zdcDigitsCollectionKey;

    // ntuple variables

    /** Basic track parameters */
    std::vector<std::vector<int> > *m_digits_gain0_delay0;
    std::vector<std::vector<int> > *m_digits_gain0_delay1;
    std::vector<std::vector<int> > *m_digits_gain1_delay0;
    std::vector<std::vector<int> > *m_digits_gain1_delay1;
  };
}

#endif
