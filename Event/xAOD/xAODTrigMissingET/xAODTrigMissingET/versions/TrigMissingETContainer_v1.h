/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMissingET_TrigMissingETCONTAINER_V1_H
#define xAODTrigMissingET_TrigMissingETCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "xAODTrigMissingET/versions/TrigMissingET_v1.h"

namespace xAOD {
  /// Datavector of all trig missing et objects.
  ///
  /// Even though there is only a single TrigMissingET object per
  /// event in the trigger software, the architecture of the trigger means
  /// that one needs to save it in a vector.
  typedef DataVector<xAOD::TrigMissingET_v1> TrigMissingETContainer_v1;

}

#endif
