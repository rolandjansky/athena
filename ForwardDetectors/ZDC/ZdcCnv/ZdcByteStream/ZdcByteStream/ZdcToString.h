/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ZDCTOSTRING_H
#define TRIGT1CALOBYTESTREAM_ZDCTOSTRING_H
#include <string>
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "xAODTrigL1Calo/CPMTower.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"

//namespace LVL1BS {

std::string ZdcToString(const xAOD::TriggerTower& tt);
std::string ZdcToString(const xAOD::TriggerTowerContainer& container);
std::string ZdcToString(const xAOD::CPMTower& tt);
std::string ZdcToString(const xAOD::CPMTowerContainer& container);

//}

#endif
