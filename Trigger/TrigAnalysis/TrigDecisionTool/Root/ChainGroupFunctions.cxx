/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : ChainGroupFunctions
 *
 * @brief Chain Group management
 *
 * @author Michael Begel  <michael.begel@cern.ch>  - Brookhaven National Laboratory
 * @author Tomasz Bold    <Tomasz.Bold@cern.ch>    - UC Irvine - AGH Krakow
 * @author Joerg Stelzer  <Joerg.Stelzer@cern.ch>  - DESY
 *
 ***********************************************************************************/

#include <string>
#include <vector>
//#include <iostream>

#include "TrigDecisionTool/ChainGroupFunctions.h"
#include "TrigDecisionTool/TDTUtilities.h"

Trig::ChainGroupFunctions::ChainGroupFunctions() {}

Trig::ChainGroupFunctions::~ChainGroupFunctions() {}

void Trig::ChainGroupFunctions::ChainGroupInitialize() {
  // all triggers
  getChainGroup(".*");
  // all L1 triggers
  getChainGroup("L1_.*");
  // all L2 triggers
  getChainGroup("L2_.*");
  // all EF triggers
  getChainGroup("EF_.*");
  // all merged L2EF triggers
  getChainGroup("HLT_.*");
}

const Trig::ChainGroup* Trig::ChainGroupFunctions::getChainGroup(const std::vector< std::string >& triggerNames) const {
  auto searchRes = cgm()->getChainGroups().find(triggerNames);

  if ( searchRes != cgm()->getChainGroups().end()) {
    return searchRes->second;
  }
  else {
    return cgm()->createChainGroup(triggerNames);
  }
}

const Trig::ChainGroup* Trig::ChainGroupFunctions::getChainGroup(const std::string& triggerNames) const {
   return getChainGroup(Trig::convertStringToVector(triggerNames));
}

