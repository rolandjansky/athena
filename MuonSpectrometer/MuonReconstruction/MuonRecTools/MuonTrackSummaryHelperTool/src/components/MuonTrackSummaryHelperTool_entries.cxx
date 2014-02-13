
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool.h"
#include "../MuonHitSummaryTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonTrackSummaryHelperTool )
DECLARE_TOOL_FACTORY( MuonHitSummaryTool )

DECLARE_FACTORY_ENTRIES( MuonTrackSummaryHelperTool )
{
  DECLARE_TOOL( MuonTrackSummaryHelperTool )
  DECLARE_TOOL( MuonHitSummaryTool );
}




