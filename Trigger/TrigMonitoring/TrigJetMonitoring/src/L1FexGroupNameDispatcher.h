
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGJETMONITORING_L1FEXGROUPNAMEDISPATCHER_H
#define TRIGJETMONITORING_L1FEXGROUPNAMEDISPATCHER_H

#include "./TrigL1FexJetMonitorTool.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/gFexJetRoIContainer.h"

std::string
groupNameDispatcher(TrigL1FexJetMonitorTool<xAOD::jFexSRJetRoIContainer>*);

std::string
groupNameDispatcher(TrigL1FexJetMonitorTool<xAOD::jFexLRJetRoIContainer>*);


std::string
groupNameDispatcher(TrigL1FexJetMonitorTool<xAOD::gFexJetRoIContainer>*);

#endif
