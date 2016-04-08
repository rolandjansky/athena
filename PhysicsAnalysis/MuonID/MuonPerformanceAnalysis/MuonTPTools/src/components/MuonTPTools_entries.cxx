// MuonTPTools_entries.cxx

#include "MuonTPTools/MuonTPTool.h"

#include "MuonTPTools/MuonTPSelectionTool.h"
#include "MuonTPTools/DiMuonTPSelectionTool.h"

#include "MuonTPTools/MuonTPEfficiencyTool.h"
#include "MuonTPTools/MuonRecoTPEfficiencyTool.h"
#include "MuonTPTools/MuonTrigTPEfficiencyTool.h"
#include "MuonTPTools/MuonIsolTPEfficiencyTool.h"

#include "MuonTPTools/MuonTPPlotTool.h"
#include "MuonTPTools/DiMuonTPPlotTool.h"

#include "MuonTPTools/MuonTPTreeTool.h"
#include "MuonTPTools/DiMuonTPTreeTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(MuonTPTool)

// DECLARE_TOOL_FACTORY(MuonTPSelectionTool)
DECLARE_TOOL_FACTORY(DiMuonTPSelectionTool)

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
  
//   DECLARE_TOOL(MuonTPSelectionTool)
  DECLARE_TOOL(DiMuonTPSelectionTool)
  
//   DECLARE_TOOL(MuonTPEfficiencyTool)
  DECLARE_TOOL(MuonRecoTPEfficiencyTool)
  DECLARE_TOOL(MuonTrigTPEfficiencyTool)
  DECLARE_TOOL(MuonIsolTPEfficiencyTool)
  
//   DECLARE_TOOL(MuonTPPlotTool)
  DECLARE_TOOL(DiMuonTPPlotTool)
  
//   DECLARE_TOOL(MuonTPTreeTool)
  DECLARE_TOOL(DiMuonTPTreeTool)
  
}

