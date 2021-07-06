/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_GBLOCKCONTAINER_H
#define XAODTRIGL1CALO_GBLOCKCONTAINER_H

// Local include(s):
#include "xAODTrigL1Calo/versions/GBlockContainer_v1.h"
#include "xAODTrigL1Calo/GBlock.h"

namespace xAOD
{
  /// Define the latest version of the GBlock container
  typedef GBlockContainer_v1 GBlockContainer;
} // namespace xAOD

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF(xAOD::GBlockContainer, 1133057962, 1)

#endif // XAODTRIGL1CALO_GBLOCKCONTAINER_H