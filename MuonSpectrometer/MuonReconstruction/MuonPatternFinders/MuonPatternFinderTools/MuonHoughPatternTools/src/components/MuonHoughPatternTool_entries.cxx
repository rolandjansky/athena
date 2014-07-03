#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonHoughPatternTools/MuonHoughPatternTool.h"
#include "MuonHoughPatternTools/MuonHoughPatternFinderTool.h"
#include "MuonHoughPatternTools/MuonLayerHoughTool.h"
#include "../MuonLayerHoughAlg.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonHoughPatternTool )
DECLARE_TOOL_FACTORY( MuonHoughPatternFinderTool )
DECLARE_TOOL_FACTORY( MuonLayerHoughTool )
DECLARE_ALGORITHM_FACTORY( MuonLayerHoughAlg )

DECLARE_FACTORY_ENTRIES( MuonHoughPatternTools ) 
{
  DECLARE_TOOL( MuonHoughPatternTool )
  DECLARE_TOOL( MuonLayerHoughTool )
  DECLARE_TOOL( MuonHoughPatternFinderTool )
  DECLARE_ALGORITHM( MuonLayerHoughAlg )
}
