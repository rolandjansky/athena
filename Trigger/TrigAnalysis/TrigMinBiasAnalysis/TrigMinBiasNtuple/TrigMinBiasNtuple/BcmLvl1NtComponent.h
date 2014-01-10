/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_BCMLVL1NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_BCMLVL1NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"

namespace NtComponent {
  class BcmLvl1NtComponent: public NtupleComponent {
  public:
    BcmLvl1NtComponent(NtupleAlgorithm *mainAlg,
		       NtComponentParameters parameters);
    virtual ~BcmLvl1NtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_bcmCtpKey;

    // NTUPLE variables
    unsigned int m_bcmCableWord;
  };
}

#endif
