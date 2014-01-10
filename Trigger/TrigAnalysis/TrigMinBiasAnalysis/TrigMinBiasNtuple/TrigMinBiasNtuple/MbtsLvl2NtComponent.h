/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIASNTUPLE_MBTSLVL2NTCOMPONENT_H
#define TRIGMINBIASNTUPLE_MBTSLVL2NTCOMPONENT_H

#include "TrigNtupleBase/NtupleComponent.h"
#include <vector>

namespace NtComponent {
  class MbtsLvl2NtComponent: public NtupleComponent {
  public:
    MbtsLvl2NtComponent(NtupleAlgorithm *mainAlg,
			NtComponentParameters parameters);
    virtual ~MbtsLvl2NtComponent();
    
    StatusCode book(void);
    StatusCode fill(void);

  private:
    std::string m_mbtsLvl2ContainerName;

    // NTUPLE variables
    unsigned int m_nElements;
    unsigned int m_trigger_nenergies;
    std::vector<float>* m_triggerEnergies;
    unsigned int m_trigger_ntimes;
    std::vector<float>* m_triggerTimes;
  };
}

#endif
