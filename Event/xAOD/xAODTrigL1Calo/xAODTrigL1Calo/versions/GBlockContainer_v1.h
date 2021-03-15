/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_VERSIONS_GBLOCKCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_GBLOCKCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "GBlock_v1.h"

// Declare IParticle as a base class of GBlock_v1
DATAVECTOR_BASE(xAOD::GBlock_v1, xAOD::IParticle);

namespace xAOD
{
  typedef DataVector<GBlock_v1> GBlockContainer_v1;
}

#endif //> !XAODTRIGL1CALO_VERSIONS_GBLOCKCONTAINER_V1_H