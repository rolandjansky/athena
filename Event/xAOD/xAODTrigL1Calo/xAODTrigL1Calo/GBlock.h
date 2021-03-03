/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALO_GBLOCK_H
#define XAODTRIGL1CALO_GBLOCK_H

// Local include(s):
#include "xAODTrigL1Calo/versions/GBlock_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD
{
  /// Define the latest version of the GBlock class
  typedef GBlock_v1 GBlock;
} // namespace xAOD

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF(xAOD::GBlock, 224941910, 1)

#endif // XAODTRIGL1CALO_GBLOCK_H