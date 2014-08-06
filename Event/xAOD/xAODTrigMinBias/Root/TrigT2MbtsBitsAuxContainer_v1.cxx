/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigMinBias/versions/TrigT2MbtsBitsAuxContainer_v1.h"

namespace xAOD {

  TrigT2MbtsBitsAuxContainer_v1::TrigT2MbtsBitsAuxContainer_v1 ()
    : AuxContainerBase() {

    AUX_VARIABLE ( triggerEnergies );
    AUX_VARIABLE ( triggerTimes );
  }

}
