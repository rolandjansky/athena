#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonCandidateTool.h"
#include "../MuonCombinedTool.h"
#include "../MuonCombinedDebuggerTool.h"
#include "../MuonCreatorTool.h"
#include "../MuonDressingTool.h"
#include "../MuonCombinedStacoTagTool.h"
#include "../MuonCombinedFitTagTool.h"
#include "../MuonSegmentTagTool.h"
#include "../MuGirlTagTool.h"
#include "../MuonCaloTagTool.h"
#include "../TrackSegmentAssociationTool.h"

using namespace MuonCombined;
using namespace Muon;
DECLARE_TOOL_FACTORY( MuonCandidateTool )
DECLARE_TOOL_FACTORY( MuonCombinedTool )
DECLARE_TOOL_FACTORY( MuonCombinedDebuggerTool )
DECLARE_TOOL_FACTORY( MuonCreatorTool )
DECLARE_TOOL_FACTORY( MuonDressingTool )
DECLARE_TOOL_FACTORY( MuonCombinedStacoTagTool )
DECLARE_TOOL_FACTORY( MuonCombinedFitTagTool )
DECLARE_TOOL_FACTORY( MuonSegmentTagTool )
DECLARE_TOOL_FACTORY( MuGirlTagTool )
DECLARE_TOOL_FACTORY( MuonCaloTagTool )
DECLARE_TOOL_FACTORY( TrackSegmentAssociationTool )

DECLARE_FACTORY_ENTRIES( MuonCombinedBaseTools )
{
  DECLARE_TOOL( MuonCandidateTool )
  DECLARE_TOOL( MuonCombinedTool )
  DECLARE_TOOL( MuonCombinedDebuggerTool )
  DECLARE_TOOL( MuonCreatorTool )
  DECLARE_TOOL( MuonDressingTool )
  DECLARE_TOOL( MuonCombinedStacoTagTool )
  DECLARE_TOOL( MuonCombinedFitTagTool )
  DECLARE_TOOL( MuonSegmentTagTool )
  DECLARE_TOOL( MuGirlTagTool )
  DECLARE_TOOL( MuonCaloTagTool )
  DECLARE_TOOL( TrackSegmentAssociationTool )
}

