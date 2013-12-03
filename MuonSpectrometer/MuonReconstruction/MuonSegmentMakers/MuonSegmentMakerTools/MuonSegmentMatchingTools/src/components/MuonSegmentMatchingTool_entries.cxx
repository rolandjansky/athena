#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonSegmentInOverlapResolvingTool.h"
#include "../MuonSegmentMatchingTool.h"
#include "../MuonSegmentPairMatchingTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonSegmentInOverlapResolvingTool )
DECLARE_TOOL_FACTORY( MuonSegmentMatchingTool )
DECLARE_TOOL_FACTORY( MuonSegmentPairMatchingTool )

DECLARE_FACTORY_ENTRIES( MuonSegmentMatchingTools ) 
{
  DECLARE_TOOL( MuonSegmentInOverlapResolvingTool )
  DECLARE_TOOL( MuonSegmentMatchingTool )
  DECLARE_TOOL( MuonSegmentPairMatchingTool )
}
