/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CAL_CPMTowerCOLLECTION_H
#define TRIGT1CAL_CPMTowerCOLLECTION_H

#include "AthContainers/DataVector.h"
#include "TrigT1CaloEvent/CPMTower.h"

/** Container class for CPMTowers.
    CPMTower contains the Trigger Tower information received by the
    Cluster Processor Modules. */

typedef DataVector<LVL1::CPMTower> CPMTowerCollection;

#endif
