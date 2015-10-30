#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonInsideOutRecoTool.h"
#include "../MuonStauRecoTool.h"
#include "../MuonLayerSegmentMatchingTool.h"
#include "../MuonLayerAmbiguitySolverTool.h"
#include "../MuonCandidateTrackBuilderTool.h"

using namespace Muon;
using namespace MuonCombined;

DECLARE_TOOL_FACTORY( MuonInsideOutRecoTool )
DECLARE_TOOL_FACTORY( MuonStauRecoTool )
DECLARE_TOOL_FACTORY( MuonLayerSegmentMatchingTool )
DECLARE_TOOL_FACTORY( MuonLayerAmbiguitySolverTool )
DECLARE_TOOL_FACTORY( MuonCandidateTrackBuilderTool )

DECLARE_FACTORY_ENTRIES( MuonCombinedTrackFindingTools )
{

  DECLARE_TOOL( MuonInsideOutRecoTool )
  DECLARE_TOOL( MuonStauRecoTool )
  DECLARE_TOOL( MuonLayerSegmentMatchingTool )
  DECLARE_TOOL( MuonLayerAmbiguitySolverTool )
  DECLARE_TOOL( MuonCandidateTrackBuilderTool )
}
