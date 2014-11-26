// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTowerContainer_v2.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERCONTAINER_V2_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTrigL1Calo/versions/TriggerTower_v2.h"

namespace xAOD {
   /// Define the TriggerTower as a simple DataVector
   typedef DataVector< xAOD::TriggerTower_v2 > TriggerTowerContainer_v2;
}

#endif // XAODTRIGL1CALO_VERSIONS_TRIGGERTOWERCONTAINER_V2_H
