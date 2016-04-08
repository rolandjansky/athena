// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_SCTRAWHITVALIDATIONAUXCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_SCTRAWHITVALIDATIONAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// Core include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthLinks/ElementLink.h"

namespace xAOD {
  class SCTRawHitValidationAuxContainer_v1 : public AuxContainerBase {
  public:
    SCTRawHitValidationAuxContainer_v1();
  private:
    std::vector<uint64_t> identifier;
    std::vector<uint32_t> dataword;
  };
}

// Set up StoreGate inheritance for the class
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::SCTRawHitValidationAuxContainer_v1, xAOD::AuxContainerBase);

#endif // XAODTRACKING_VERSIONS_SCTRAWHITVALIDATIONAUXCONTAINER_V1_H
