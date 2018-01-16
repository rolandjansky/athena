#ifndef TRIGT1CAL_JETCMXTOPODATACOLLECTION_H
#define TRIGT1CAL_JETCMXTOPODATACOLLECTION_H

#include "AthContainers/DataVector.h"
#include "TrigT1CaloEvent/JetCMXTopoData.h"

/** Container class for JetCMXTopoData objects.
    JetCMXTopoData contains the TOB data from a single Jet CMX
    to L1Topo  */

using namespace LVL1;
typedef DataVector<JetCMXTopoData> JetCMXTopoDataCollection;

#endif
