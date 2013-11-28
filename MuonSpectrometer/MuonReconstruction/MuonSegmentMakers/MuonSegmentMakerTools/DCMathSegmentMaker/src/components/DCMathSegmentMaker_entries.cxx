
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../DCMathSegmentMaker.h"
#include "../MdtMathSegmentFinder.h"
#include "../MuonSegmentFittingTool.h"
#include "../MuonClusterSegmentFinderTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( DCMathSegmentMaker )
DECLARE_TOOL_FACTORY( MdtMathSegmentFinder )
DECLARE_TOOL_FACTORY( MuonSegmentFittingTool )
DECLARE_TOOL_FACTORY( MuonClusterSegmentFinderTool )

DECLARE_FACTORY_ENTRIES( DCMathSegmentMaker )
{
  DECLARE_TOOL( DCMathSegmentMaker )
  DECLARE_TOOL( MdtMathSegmentFinder )
  DECLARE_TOOL( MuonSegmentFittingTool )
  DECLARE_TOOL( MuonClusterSegmentFinderTool )
}




