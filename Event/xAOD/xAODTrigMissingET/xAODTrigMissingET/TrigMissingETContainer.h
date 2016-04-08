/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMissingET_TrigMissingETCONTAINER_H
#define xAODTrigMissingET_TrigMissingETCONTAINER_H

#include "xAODTrigMissingET/versions/TrigMissingETContainer_v1.h"
#include "xAODTrigMissingET/TrigMissingET.h"

namespace xAOD {
  /// DataVector of TrigMissingET - the current version
  typedef TrigMissingETContainer_v1 TrigMissingETContainer;
}

#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (xAOD::TrigMissingETContainer, 1134334, 1)

#endif
