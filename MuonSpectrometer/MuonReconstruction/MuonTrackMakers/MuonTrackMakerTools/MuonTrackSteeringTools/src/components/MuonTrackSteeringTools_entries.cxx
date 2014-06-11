#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonTrackSteering.h"
#include "../MuPatCandidateTool.h"
#include "../MuPatHitTool.h"
#include "../MooCandidateMatchingTool.h"
#include "../MooTrackFitter.h"
#include "../MooTrackBuilder.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonTrackSteering )
DECLARE_TOOL_FACTORY( MuPatHitTool )
DECLARE_TOOL_FACTORY( MuPatCandidateTool )
DECLARE_TOOL_FACTORY( MooCandidateMatchingTool )
DECLARE_TOOL_FACTORY( MooTrackFitter )
DECLARE_TOOL_FACTORY( MooTrackBuilder )

DECLARE_FACTORY_ENTRIES( MuonTrackSteeringTools ) 
{
  DECLARE_TOOL( MuonTrackSteering )
  DECLARE_TOOL( MuPatHitTool )
  DECLARE_TOOL( MuPatCandidateTool )
  DECLARE_TOOL( MooCandidateMatchingTool )
  DECLARE_TOOL( MooTrackFitter )
  DECLARE_TOOL( MooTrackBuilder )
}
