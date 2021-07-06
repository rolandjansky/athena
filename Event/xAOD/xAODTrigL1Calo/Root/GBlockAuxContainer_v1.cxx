/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigL1Calo/versions/GBlockAuxContainer_v1.h"

namespace xAOD
{
  GBlockAuxContainer_v1::GBlockAuxContainer_v1()
      : AuxContainerBase()
  {
    AUX_VARIABLE(eta);
    AUX_VARIABLE(phi);
    AUX_VARIABLE(deta);
    AUX_VARIABLE(dphi);
    AUX_VARIABLE(et);
    AUX_VARIABLE(area);
    AUX_VARIABLE(seedJGTower);
    AUX_VARIABLE(JGTowers);
  }
} // namespace xAOD