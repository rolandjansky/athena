#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonLayerSegmentFinderTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonLayerSegmentFinderTool )

DECLARE_FACTORY_ENTRIES( MuonLayerSegmentMakerTools ) 
{
  DECLARE_TOOL( MuonLayerSegmentFinderTool )
}
