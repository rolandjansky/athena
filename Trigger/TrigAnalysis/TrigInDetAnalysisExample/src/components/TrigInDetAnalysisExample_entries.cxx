#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigInDetAnalysisExample/TrigTestMonToolAC.h"
#include "TrigInDetAnalysisExample/TrigTestPhysValMon.h"


DECLARE_TOOL_FACTORY(TrigTestMonToolAC)
DECLARE_TOOL_FACTORY(TrigTestBase)
DECLARE_TOOL_FACTORY(TrigTestPhysValMon)
DECLARE_FACTORY_ENTRIES( TrigInDetAnalysisExample ) {
  DECLARE_TOOL(TrigTestMonToolAC)
  DECLARE_TOOL(TrigTestBase)
  DECLARE_TOOL(TrigTestPhysValMon)
}

