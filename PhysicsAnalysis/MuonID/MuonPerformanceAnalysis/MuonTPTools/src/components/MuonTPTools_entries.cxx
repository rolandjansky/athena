// MuonTPTools_entries.cxx

#include "MuonTPTools/MuonTPTool.h"
#include "MuonTPTools/MuonTPSelectionTool.h"
#include "MuonTPTools/MuonTPEfficiencyTool.h"
#include "MuonTPTools/ZmumuMuonTPPlotTool.h"
#include "MuonTPTools/MuonTPPlotTool.h"
#include "MuonTPTools/ZmumuMuonTPSelectionTool.h"
#include "MuonTPTools/ZmumuMuonTPEfficiencyTool.h"
#include "MuonTPTools/JPsiMuonTPPlotTool.h"
#include "MuonTPTools/JPsiMuonTPSelectionTool.h"
#include "MuonTPTools/JPsiMuonTPEfficiencyTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(MuonTPTool)
DECLARE_TOOL_FACTORY(MuonTPSelectionTool)
DECLARE_TOOL_FACTORY(MuonTPEfficiencyTool)
DECLARE_TOOL_FACTORY(ZmumuMuonTPSelectionTool)
DECLARE_TOOL_FACTORY(MuonTPPlotTool)
DECLARE_TOOL_FACTORY(ZmumuMuonTPPlotTool)
DECLARE_TOOL_FACTORY(ZmumuMuonTPEfficiencyTool)
DECLARE_TOOL_FACTORY(JPsiMuonTPSelectionTool)
DECLARE_TOOL_FACTORY(JPsiMuonTPPlotTool)
DECLARE_TOOL_FACTORY(JPsiMuonTPEfficiencyTool)

DECLARE_FACTORY_ENTRIES(MuonTPTools) {
  DECLARE_TOOL(MuonTPTool)
  DECLARE_TOOL(MuonTPSelectionTool)
  DECLARE_TOOL(MuonTPEfficiencyTool)
  DECLARE_TOOL(ZmumuMuonTPSelectionTool)
  DECLARE_TOOL(ZmumuMuonTPPlotTool)
  DECLARE_TOOL(ZmumuMuonTPEfficiencyTool)
  DECLARE_TOOL(MuonTPPlotTool)
  DECLARE_TOOL(JPsiMuonTPSelectionTool)
  DECLARE_TOOL(JPsiMuonTPPlotTool)
  DECLARE_TOOL(JPsiMuonTPEfficiencyTool)
}

