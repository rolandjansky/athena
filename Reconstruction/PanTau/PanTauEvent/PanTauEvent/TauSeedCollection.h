/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUEVENT_TAUSEEDCOLLECTION_H
#define PANTAUEVENT_TAUSEEDCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "PanTauEvent/TauSeed.h"

namespace PanTau {
  typedef DataVector<PanTau::TauSeed> TauSeedCollection;
}

CLASS_DEF(PanTau::TauSeedCollection, 1293064487, 1)

#endif //PANTAUEVENT_TAUSEEDCOLLECTION_H
