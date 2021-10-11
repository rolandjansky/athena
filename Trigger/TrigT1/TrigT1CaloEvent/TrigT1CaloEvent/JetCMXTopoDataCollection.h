/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CAL_JETCMXTOPODATACOLLECTION_H
#define TRIGT1CAL_JETCMXTOPODATACOLLECTION_H

#include "AthContainers/DataVector.h"
#include "TrigT1CaloEvent/JetCMXTopoData.h"

/** Container class for JetCMXTopoData objects.
    JetCMXTopoData contains the TOB data from a single Jet CMX
    to L1Topo  */

typedef DataVector<LVL1::JetCMXTopoData> JetCMXTopoDataCollection;

#endif
