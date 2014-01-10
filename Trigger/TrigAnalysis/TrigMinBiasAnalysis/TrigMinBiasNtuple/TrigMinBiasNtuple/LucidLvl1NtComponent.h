/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_LUCIDLVL1NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_LUCIDLVL1NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"

namespace NtComponent {
  class LucidLvl1NtComponent: public NtupleComponent {
  public:
    LucidLvl1NtComponent(NtupleAlgorithm *mainAlg,
			 NtComponentParameters parameters);
    virtual ~LucidLvl1NtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_lucidCtpKey;

    // NTUPLE variables
    unsigned int m_lucidCableWord;
  };
}

#endif
