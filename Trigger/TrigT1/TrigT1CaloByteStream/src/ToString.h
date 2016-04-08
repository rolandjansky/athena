/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_TOSTRING_H
#define TRIGT1CALOBYTESTREAM_TOSTRING_H
#include <string>
#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include "xAODTrigL1Calo/CPMTower.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"

namespace LVL1BS {
	std::string ToString(const xAOD::TriggerTower& tt);
	std::string ToString(const xAOD::TriggerTowerContainer& container);
  	std::string ToString(const xAOD::CPMTower& tt);
  	std::string ToString(const xAOD::CPMTowerContainer& container);

}

#endif
