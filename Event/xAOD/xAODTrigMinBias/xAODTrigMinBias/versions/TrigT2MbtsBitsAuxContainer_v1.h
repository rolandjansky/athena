/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigT2MbtsBitsAUXCONTAINTER_V1_H
#define xAODTrigMinBias_TrigT2MbtsBitsAUXCONTAINTER_V1_H

#include "xAODCore/AuxContainerBase.h"

#include <vector>

namespace xAOD {

  class TrigT2MbtsBitsAuxContainer_v1 : public AuxContainerBase {

  public:
    TrigT2MbtsBitsAuxContainer_v1();

  private:
    std::vector<std::vector<float> > triggerEnergies;
    std::vector<std::vector<float> > triggerTimes;
  };

}

#include "xAODCore/BaseInfo.h"
SG_BASE(   xAOD::TrigT2MbtsBitsAuxContainer_v1, xAOD::AuxContainerBase );

#endif
