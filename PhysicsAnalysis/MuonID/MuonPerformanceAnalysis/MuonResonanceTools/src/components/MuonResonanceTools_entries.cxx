// AsgExampleTools_entries.cxx

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonResonanceTools/MuonResonanceTool.h"
#include "MuonResonanceTools/MuonResonanceSelectionTool.h"
#include "MuonResonanceTools/MuonResonancePairingTool.h"


DECLARE_TOOL_FACTORY(MuonResonanceTool)
DECLARE_TOOL_FACTORY(MuonResonanceSelectionTool)
DECLARE_TOOL_FACTORY(MuonResonancePairingTool)


DECLARE_FACTORY_ENTRIES(MuonPerformanceTools) {

  DECLARE_TOOL(MuonResonanceTool)
  DECLARE_TOOL(MuonResonanceSelectionTool)
  DECLARE_TOOL(MuonResonancePairingTool)
}

