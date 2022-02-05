/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//simple includes to force the CLASS_DEF etc to be encountered during compile

#include "xAODTrigger/BunchConfContainer.h"
#include "xAODTrigger/BunchConfAuxContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "xAODTrigger/JetEtRoI.h"
#include "xAODTrigger/JetEtRoIAuxInfo.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODTrigger/TriggerMenuJsonContainer.h"
#include "xAODTrigger/TriggerMenuJsonAuxContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/MuonRoIAuxContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigDecisionAuxInfo.h"
#include "xAODTrigger/TrigNavigation.h"
#include "xAODTrigger/TrigNavigationAuxInfo.h"
#include "xAODTrigger/TriggerMenuContainer.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"
#include "xAODTrigger/TrigPassBitsContainer.h"
#include "xAODTrigger/TrigPassBitsAuxContainer.h"
#include "xAODTrigger/RoiDescriptorStore.h"
#include "xAODTrigger/RoiDescriptorStoreAuxInfo.h"
#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexEMRoIAuxContainer.h"

#include "xAODTrigger/eFexTauRoIContainer.h"
#include "xAODTrigger/eFexTauRoIAuxContainer.h"

#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexSRJetRoIAuxContainer.h"

#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIAuxContainer.h"

#include "xAODTrigger/jFexTauRoIContainer.h"
#include "xAODTrigger/jFexTauRoIAuxContainer.h"

#include "xAODTrigger/jFexSumETRoIContainer.h"
#include "xAODTrigger/jFexSumETRoIAuxContainer.h"

#include "xAODTrigger/jFexMETRoIContainer.h"
#include "xAODTrigger/jFexMETRoIAuxContainer.h"

#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/gFexJetRoIAuxContainer.h"
#include "xAODTrigger/gFexGlobalRoIContainer.h"
#include "xAODTrigger/gFexGlobalRoIAuxContainer.h"

#include "xAODTrigger/L1TopoSimResultsContainer.h"
#include "xAODTrigger/L1TopoSimResultsAuxContainer.h"
