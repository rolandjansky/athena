
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./L1FexGroupNameDispatcher.h"

std::string
groupNameDispatcher(TrigL1FexJetMonitorTool<xAOD::jFexSRJetRoIContainer>*) {
  return "TrigL1JFexSRJetMonitor";
}

std::string
groupNameDispatcher(TrigL1FexJetMonitorTool<xAOD::jFexLRJetRoIContainer>*) {
  return "TrigL1JFexLRJetMonitor";
}


std::string
groupNameDispatcher(TrigL1FexJetMonitorTool<xAOD::gFexJetRoIContainer>*) {
  return "TrigL1GFexJetMonitor";
}
