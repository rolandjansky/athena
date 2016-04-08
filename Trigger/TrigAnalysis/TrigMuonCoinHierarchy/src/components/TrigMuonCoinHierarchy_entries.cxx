#include "GaudiKernel/DeclareFactoryEntries.h" 

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

DECLARE_FACTORY_ENTRIES(TrigMuonCoinHierarchy) {
  DECLARE_NAMESPACE_SERVICE(Trigger, MuonRoiChainSvc)
  DECLARE_NAMESPACE_TOOL(Trigger, MuonRoiChainFindTool)
  DECLARE_NAMESPACE_ALGORITHM(Trigger, MuonRoiChainTestAlg)

  DECLARE_NAMESPACE_SERVICE(Trigger, TgcCoinHierarchySvc)
  DECLARE_NAMESPACE_TOOL(Trigger, TgcCoinHierarchyClassifyTool)
  DECLARE_NAMESPACE_TOOL(Trigger, TgcCoinHierarchyFindTool)
  DECLARE_NAMESPACE_ALGORITHM(Trigger, TgcCoinHierarchyTestAlg)
  DECLARE_NAMESPACE_ALGORITHM(Trigger, TgcCoinHierarchyValidation)
} 
