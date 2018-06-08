/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CAL_CPCMXTOPODATACOLLECTION_H
#define TRIGT1CAL_CPCMXTOPODATACOLLECTION_H

#include "DataModel/DataVector.h"
#include "TrigT1CaloEvent/CPCMXTopoData.h"

/** Container class for CPCMXTopoData objects.
    CPCMXTopoData contains the TOB data from a single CP CMX
    to L1Topo  */

using namespace LVL1;
typedef DataVector<CPCMXTopoData> CPCMXTopoDataCollection;

#endif
