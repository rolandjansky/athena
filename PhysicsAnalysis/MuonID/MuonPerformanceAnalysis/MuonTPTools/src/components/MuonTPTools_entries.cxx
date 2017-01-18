// MuonTPTools_entries.cxx

#include "MuonTPTools/BadMuonVetoHelperTool.h"
#include "MuonTPTools/MuonTPTool.h"
#include "MuonTPTools/MuonTPExtrapolationTool.h"
#include "MuonTPTools/IDTrackIsolationDecoratorTool.h"
#include "MuonTPTools/IDTrackCaloDepositsDecoratorTool.h"
#include "MuonTPTools/MuonTPTrigUtils.h"

#include "MuonTPTools/MuonTPSelectionTool.h"
#include "MuonTPTools/DiMuonTPSelectionTool.h"
#include "MuonTPTools/HighPtSingleMuonSelectionTool.h"

#include "MuonTPTools/MuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonRecoTPEfficiencyTool.h"
#include "MuonTPTools/MuonTrigTPEfficiencyTool.h"
#include "MuonTPTools/MuonIsolTPEfficiencyTool.h"
#include "MuonTPTools/MuonTPPlotTool.h"
#include "MuonTPTools/DiMuonTPPlotTool.h"

#include "MuonTPTools/MuonTPTreeTool.h"
#include "MuonTPTools/DiMuonTPTreeTool.h"
#include "MuonTPTools/MuonTPVertexHelper.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(MuonTPTool)
DECLARE_TOOL_FACTORY(MuonTPExtrapolationTool)
DECLARE_TOOL_FACTORY(IDTrackIsolationDecoratorTool)
DECLARE_TOOL_FACTORY(IDTrackCaloDepositsDecoratorTool)
DECLARE_TOOL_FACTORY(BadMuonVetoHelperTool)
DECLARE_TOOL_FACTORY(MuonTPTrigUtils)
DECLARE_TOOL_FACTORY(MuonTPVertexHelper)

// DECLARE_TOOL_FACTORY(MuonTPSelectionTool)
DECLARE_TOOL_FACTORY(DiMuonTPSelectionTool)
DECLARE_TOOL_FACTORY(HighPtSingleMuonSelectionTool)

// DECLARE_TOOL_FACTORY(MuonTPEfficiencyTool)
DECLARE_TOOL_FACTORY(MuonRecoTPEfficiencyTool)
DECLARE_TOOL_FACTORY(MuonTrigTPEfficiencyTool)
DECLARE_TOOL_FACTORY(MuonIsolTPEfficiencyTool)

// DECLARE_TOOL_FACTORY(MuonTPPlotTool)
DECLARE_TOOL_FACTORY(DiMuonTPPlotTool)
// DECLARE_TOOL_FACTORY(JPsiMuonTPPlotTool)

// DECLARE_TOOL_FACTORY(MuonTPTreeTool)
DECLARE_TOOL_FACTORY(DiMuonTPTreeTool)

DECLARE_FACTORY_ENTRIES(MuonTPTools) {
  DECLARE_TOOL(MuonTPTool)
  DECLARE_TOOL(MuonTPVertexHelper)
  DECLARE_TOOL(MuonTPExtrapolationTool)
  DECLARE_TOOL(IDTrackIsolationDecoratorTool)
  DECLARE_TOOL(IDTrackCaloDepositsDecoratorTool)
  DECLARE_TOOL(BadMuonVetoHelperTool)
  DECLARE_TOOL(MuonTPTrigUtils)
  
//   DECLARE_TOOL(MuonTPSelectionTool)
  DECLARE_TOOL(DiMuonTPSelectionTool)
  DECLARE_TOOL(HighPtSingleMuonSelectionTool)
  
//   DECLARE_TOOL(MuonTPEfficiencyTool)
  DECLARE_TOOL(MuonRecoTPEfficiencyTool)
  DECLARE_TOOL(MuonTrigTPEfficiencyTool)
  DECLARE_TOOL(MuonIsolTPEfficiencyTool)
  
//   DECLARE_TOOL(MuonTPPlotTool)
  DECLARE_TOOL(DiMuonTPPlotTool)
  
//   DECLARE_TOOL(MuonTPTreeTool)
  DECLARE_TOOL(DiMuonTPTreeTool)
  
}

