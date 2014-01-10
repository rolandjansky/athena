/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_MBTSLVL1NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_MBTSLVL1NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"

namespace NtComponent {
  class MbtsLvl1NtComponent: public NtupleComponent {
  public:
    MbtsLvl1NtComponent(NtupleAlgorithm *mainAlg,
			NtComponentParameters parameters);
    virtual ~MbtsLvl1NtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_mbtsCtpAinputContainerName;
    std::string m_mbtsCtpCinputContainerName;

    // NTUPLE variables
    unsigned int m_mbtsCableWordA;
    unsigned int m_mbtsCableWordC;
  };
}

#endif
