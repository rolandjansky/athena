/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTracking/versions/SCTRawHitValidationAuxContainer_v1.h"

namespace xAOD {
  SCTRawHitValidationAuxContainer_v1::SCTRawHitValidationAuxContainer_v1()
    : AuxContainerBase()
  {
    AUX_VARIABLE(identifier);
    AUX_VARIABLE(dataword);
  }
}
