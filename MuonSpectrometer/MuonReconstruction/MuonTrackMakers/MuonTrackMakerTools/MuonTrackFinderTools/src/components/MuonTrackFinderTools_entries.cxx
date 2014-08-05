#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonTrackCleaner.h"
#include "../MuonChamberHoleRecoveryTool.h"
#include "../MuonSegmentRegionRecoveryTool.h"
#include "../MuonTrackToSegmentTool.h"
#include "../MuonTrackSelectorTool.h"
#include "../MuonSeededSegmentFinder.h"
#include "../MuonTrackScoringTool.h"
#include "../MuonTrackExtrapolationTool.h"
#include "../MuonRefitTool.h"
#include "../MuonErrorOptimisationTool.h"

using namespace Muon;

DECLARE_TOOL_FACTORY( MuonTrackCleaner )
DECLARE_TOOL_FACTORY( MuonChamberHoleRecoveryTool )
DECLARE_TOOL_FACTORY( MuonSegmentRegionRecoveryTool )
DECLARE_TOOL_FACTORY( MuonTrackToSegmentTool )
DECLARE_TOOL_FACTORY( MuonTrackSelectorTool )
DECLARE_TOOL_FACTORY( MuonSeededSegmentFinder )
DECLARE_TOOL_FACTORY( MuonTrackScoringTool )
DECLARE_TOOL_FACTORY( MuonTrackExtrapolationTool )
DECLARE_TOOL_FACTORY( MuonRefitTool )
DECLARE_TOOL_FACTORY( MuonErrorOptimisationTool )

DECLARE_FACTORY_ENTRIES( MuonTrackFinderTools ) 
{
  DECLARE_TOOL( MuonTrackCleaner )
  DECLARE_TOOL( MuonChamberHoleRecoveryTool )
  DECLARE_TOOL( MuonSegmentRegionRecoveryTool )
  DECLARE_TOOL( MuonTrackToSegmentTool )
  DECLARE_TOOL( MuonTrackSelectorTool )
  DECLARE_TOOL( MuonSeededSegmentFinder )
  DECLARE_TOOL( MuonTrackScoringTool )
  DECLARE_TOOL( MuonTrackExtrapolationTool )
  DECLARE_TOOL( MuonRefitTool )
  DECLARE_TOOL( MuonErrorOptimisationTool )
}
