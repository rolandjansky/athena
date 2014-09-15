#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonSegmentOverlapRemovalTool.h"
#include "../MuonSegmentCombinationCleanerTool.h"
#include "../MuonSegmentMerger.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonSegmentOverlapRemovalTool )
DECLARE_TOOL_FACTORY( MuonSegmentCombinationCleanerTool )
DECLARE_TOOL_FACTORY( MuonSegmentMerger )

DECLARE_FACTORY_ENTRIES( MuonSegmentOverlapRemovalTools ) 
{
  DECLARE_TOOL( MuonSegmentOverlapRemovalTool )
  DECLARE_TOOL( MuonSegmentCombinationCleanerTool )
  DECLARE_TOOL( MuonSegmentMerger )
}
