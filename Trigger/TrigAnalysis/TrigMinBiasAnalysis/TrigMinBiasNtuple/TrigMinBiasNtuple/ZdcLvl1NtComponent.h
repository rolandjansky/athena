/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_ZDCLVL1NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_ZDCLVL1NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"

namespace NtComponent {
  class ZdcLvl1NtComponent: public NtupleComponent {
  public:
    ZdcLvl1NtComponent(NtupleAlgorithm *mainAlg,
		       NtComponentParameters parameters);
    virtual ~ZdcLvl1NtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_zdcCtpKey;

    // NTUPLE variables
    unsigned int m_zdcCableWord;
  };
}

#endif
