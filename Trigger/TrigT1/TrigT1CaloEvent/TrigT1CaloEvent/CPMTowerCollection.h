#ifndef TRIGT1CAL_CPMTowerCOLLECTION_H
#define TRIGT1CAL_CPMTowerCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "TrigT1CaloEvent/CPMTower.h"

/** Container class for CPMTowers.
    CPMTower contains the Trigger Tower information received by the
    Cluster Processor Modules. */

using namespace LVL1;
typedef DataVector<CPMTower> CPMTowerCollection;

#endif
