#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonTrackTagTestTool.h"

//using namespace MuonCombined;

DECLARE_NAMESPACE_TOOL_FACTORY( MuonCombined, MuonTrackTagTestTool )
  
DECLARE_FACTORY_ENTRIES( MuonCombinedTestTools ) {
  DECLARE_NAMESPACE_TOOL( MuonCombined, MuonTrackTagTestTool )
}
