// MuonTPTools_entries.cxx

#include "MuonTPTools/MuonTPTool.h"
#include "MuonTPTools/MuonTPSelectionTool.h"
#include "MuonTPTools/MuonTPEfficiencyTool.h"
#include "MuonTPTools/ZmumuMuonTPPlotTool.h"
#include "MuonTPTools/MuonTPPlotTool.h"
#include "MuonTPTools/MuonTPTreeTool.h"
#include "MuonTPTools/ZmumuMuonTPTreeTool.h"
#include "MuonTPTools/ZmumuMuonTPIsolationTreeTool.h"
#include "MuonTPTools/ZmumuMuonTPSelectionTool.h"
#include "MuonTPTools/MuonRecoTPEfficiencyTool.h"
#include "MuonTPTools/MuonTrigTPEfficiencyTool.h"
#include "MuonTPTools/MuonTrigTPPlotTool.h"
#include "MuonTPTools/MuonTrigTPTreeTool.h"
#include "MuonTPTools/JPsiMuonTPPlotTool.h"
#include "MuonTPTools/JPsiMuonTPSelectionTool.h"
#include "MuonTPTools/JPsiMuonTPEfficiencyTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(MuonTPTool)
DECLARE_TOOL_FACTORY(MuonTPSelectionTool)
DECLARE_TOOL_FACTORY(MuonTPEfficiencyTool)
DECLARE_TOOL_FACTORY(ZmumuMuonTPSelectionTool)
DECLARE_TOOL_FACTORY(MuonTPPlotTool)
DECLARE_TOOL_FACTORY(MuonTPTreeTool)
DECLARE_TOOL_FACTORY(ZmumuMuonTPPlotTool)
DECLARE_TOOL_FACTORY(ZmumuMuonTPTreeTool)
DECLARE_TOOL_FACTORY(ZmumuMuonTPIsolationTreeTool)
DECLARE_TOOL_FACTORY(MuonRecoTPEfficiencyTool)
DECLARE_TOOL_FACTORY(MuonTrigTPEfficiencyTool)
DECLARE_TOOL_FACTORY(MuonTrigTPPlotTool)
DECLARE_TOOL_FACTORY(MuonTrigTPTreeTool)
DECLARE_TOOL_FACTORY(JPsiMuonTPSelectionTool)
DECLARE_TOOL_FACTORY(JPsiMuonTPPlotTool)
DECLARE_TOOL_FACTORY(JPsiMuonTPEfficiencyTool)

DECLARE_FACTORY_ENTRIES(MuonTPTools) {
  DECLARE_TOOL(MuonTPTool)
  DECLARE_TOOL(MuonTPSelectionTool)
  DECLARE_TOOL(MuonTPEfficiencyTool)
  DECLARE_TOOL(ZmumuMuonTPSelectionTool)
  DECLARE_TOOL(ZmumuMuonTPPlotTool)
  DECLARE_TOOL(ZmumuMuonTPTreeTool)
  DECLARE_TOOL(ZmumuMuonTPIsolationTreeTool)
  DECLARE_TOOL(MuonRecoTPEfficiencyTool)
  DECLARE_TOOL(MuonTrigTPEfficiencyTool)
  DECLARE_TOOL(MuonTrigTPPlotTool)
  DECLARE_TOOL(MuonTrigTPTreeTool)
  DECLARE_TOOL(MuonTPPlotTool)
  DECLARE_TOOL(MuonTPTreeTool)
  DECLARE_TOOL(JPsiMuonTPSelectionTool)
  DECLARE_TOOL(JPsiMuonTPPlotTool)
  DECLARE_TOOL(JPsiMuonTPEfficiencyTool)
}

