#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonSegmentSelectionTool.h"
#include "../MuonSegmentHitSummaryTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonSegmentSelectionTool )
DECLARE_TOOL_FACTORY( MuonSegmentHitSummaryTool )

DECLARE_FACTORY_ENTRIES( MuonSegmentSelectionTool ) 
{
  DECLARE_TOOL( MuonSegmentSelectionTool )
  DECLARE_TOOL( MuonSegmentHitSummaryTool )
}
