/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUEVENT_PANTAUSEEDCONTAINER_H
#define PANTAUEVENT_PANTAUSEEDCONTAINER_H

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "PanTauEvent/PanTauSeed.h"

namespace PanTau {
  typedef DataVector<PanTau::PanTauSeed> PanTauSeedContainer;
}

// CLASS_DEF(PanTau::PanTauSeedContainer, 1293064487, 1)
CLASS_DEF(PanTau::PanTauSeedContainer, 2013042420, 1)

#endif //PANTAUEVENT_PANTAUSEEDCONTAINER_H 

