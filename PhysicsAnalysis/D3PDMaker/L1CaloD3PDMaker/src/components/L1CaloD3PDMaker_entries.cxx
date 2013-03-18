// Problem Channel 
#include "../L1CaloProblemChannelFillerTool.h"
// PPM
#include "../L1CaloPPMFillerTool.h"
#include "../L1CaloPPMdbDataFillerTool.h"
#include "../L1CaloPPMdbMCFillerTool.h"
// CPM
#include "../L1CaloCPMHitsFillerTool.h"
#include "../L1CaloCPMRoIFillerTool.h"
#include "../L1CaloCPMTowerFillerTool.h"
// JEM
#include "../L1CaloJEMEtSumsFillerTool.h"
#include "../L1CaloJEMHitsFillerTool.h"
#include "../L1CaloJEMRoIFillerTool.h"
#include "../L1CaloJetElementFillerTool.h"
// CMM
#include "../L1CaloCMMCPHitsFillerTool.h"
#include "../L1CaloCMMEtSumsFillerTool.h"
#include "../L1CaloCMMJetHitsFillerTool.h"
#include "../L1CaloCMMRoIFillerTool.h"
// ROD
#include "../L1CaloRODHeaderFillerTool.h"
// Reprocess
#include "../ReprocessL1CaloFillerTool.h"
// No FCAL RoIs
#include "../L1CaloNoFCALRoIFillerTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloProblemChannelFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloPPMFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloPPMdbDataFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloPPMdbMCFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloCPMHitsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloCPMRoIFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloCPMTowerFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloJEMEtSumsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloJEMHitsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloJEMRoIFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloJetElementFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloCMMCPHitsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloCMMEtSumsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloCMMJetHitsFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloCMMRoIFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloRODHeaderFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,ReprocessL1CaloFillerTool)

DECLARE_NAMESPACE_TOOL_FACTORY (D3PD,L1CaloNoFCALRoIFillerTool)


DECLARE_FACTORY_ENTRIES( TrigT1CaloD3PDMaker ) {
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloProblemChannelFillerTool)
  
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloPPMFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloPPMdbDataFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloPPMdbMCFillerTool)

  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloCPMHitsFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloCPMRoIFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloCPMTowerFillerTool)

  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloJEMEtSumsFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloJEMHitsFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloJEMRoIFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloJetElementFillerTool)
    
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloCMMCPHitsFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloCMMEtSumsFillerTool)  
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloCMMJetHitsFillerTool)
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloCMMRoIFillerTool)
  
  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloRODHeaderFillerTool)

  DECLARE_NAMESPACE_TOOL (D3PD,ReprocessL1CaloFillerTool)

  DECLARE_NAMESPACE_TOOL (D3PD,L1CaloNoFCALRoIFillerTool)
}
