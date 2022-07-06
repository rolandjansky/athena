/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "../JTMContainers.h"
#include "../TrigL1FexJetMonitorTool.h"
#include "../TrigL1FexJetMonitorAlgorithm.h"

// template instantiation is done elswhere. Do no repeat it here.
extern template class TrigL1FexJetMonitorTool<JTM_JetRoIContainer>;
extern template class TrigL1FexJetMonitorTool<JTM_gFexJetRoIContainer>;
extern template class TrigL1FexJetMonitorTool<JTM_jFexLRJetRoIContainer>;
extern template class TrigL1FexJetMonitorTool<JTM_jFexSRJetRoIContainer>;

DECLARE_COMPONENT(TrigL1FexJetMonitorAlgorithm)
DECLARE_COMPONENT(TrigL1FexJetMonitorTool<JTM_JetRoIContainer>)
DECLARE_COMPONENT(TrigL1FexJetMonitorTool<JTM_gFexJetRoIContainer>)
DECLARE_COMPONENT(TrigL1FexJetMonitorTool<JTM_jFexLRJetRoIContainer>)
DECLARE_COMPONENT(TrigL1FexJetMonitorTool<JTM_jFexSRJetRoIContainer>)



