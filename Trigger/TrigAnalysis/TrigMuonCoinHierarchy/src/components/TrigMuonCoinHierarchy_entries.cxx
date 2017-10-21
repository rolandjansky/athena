
#include "TrigMuonCoinHierarchy/MuonRoiChainSvc.h" 
#include "TrigMuonCoinHierarchy/MuonRoiChainFindTool.h" 
#include "TrigMuonCoinHierarchy/MuonRoiChainTestAlg.h" 

#include "TrigMuonCoinHierarchy/TgcCoinHierarchySvc.h" 
#include "TrigMuonCoinHierarchy/TgcCoinHierarchyClassifyTool.h" 
#include "TrigMuonCoinHierarchy/TgcCoinHierarchyFindTool.h" 
#include "TrigMuonCoinHierarchy/TgcCoinHierarchyTestAlg.h" 
#include "TrigMuonCoinHierarchy/TgcCoinHierarchyValidation.h" 

DECLARE_NAMESPACE_SERVICE_FACTORY(Trigger, MuonRoiChainSvc)
DECLARE_NAMESPACE_TOOL_FACTORY(Trigger, MuonRoiChainFindTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trigger, MuonRoiChainTestAlg)

DECLARE_NAMESPACE_SERVICE_FACTORY(Trigger, TgcCoinHierarchySvc)
DECLARE_NAMESPACE_TOOL_FACTORY(Trigger, TgcCoinHierarchyClassifyTool)
DECLARE_NAMESPACE_TOOL_FACTORY(Trigger, TgcCoinHierarchyFindTool)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trigger, TgcCoinHierarchyTestAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trigger, TgcCoinHierarchyValidation)
