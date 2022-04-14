/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "../HLTJetMonTool.h"
#include "../TrigL1JetMonitorAlgorithm.h"
#include "../TrigL1JFexSRJetMonitorAlgorithm.h"
#include "../TrigL1FexJetMonitorTool.h"
#include "../TrigL1FexJetMonitorAlgorithm.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"

typedef TrigL1FexJetMonitorTool<xAOD::jFexSRJetRoIContainer>
TrigjFexSRJetRoIMonitorTool;

typedef TrigL1FexJetMonitorTool<xAOD::jFexLRJetRoIContainer>
TrigjFexLRJetRoIMonitorTool;

typedef TrigL1FexJetMonitorTool<xAOD::gFexJetRoIContainer>
TriggFexJetRoIMonitorTool;

// typedef TrigL1FexJetMonitorTool<double>
// TrigjFexSRJetRoIMonitorTool;


DECLARE_COMPONENT(HLTJetMonTool)
DECLARE_COMPONENT(TrigL1JetMonitorAlgorithm)
DECLARE_COMPONENT(TrigL1FexJetMonitorAlgorithm)
DECLARE_COMPONENT(TrigL1JFexSRJetMonitorAlgorithm)
DECLARE_COMPONENT(TrigjFexSRJetRoIMonitorTool)
DECLARE_COMPONENT(TrigjFexLRJetRoIMonitorTool)


