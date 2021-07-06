#include "../MuonChamberHoleRecoveryTool.h"
#include "../MuonErrorOptimisationTool.h"
#include "../MuonRefitTool.h"
#include "../MuonSeededSegmentFinder.h"
#include "../MuonSegmentRegionRecoveryTool.h"
#include "../MuonTrackCleaner.h"
#include "../MuonTrackExtrapolationTool.h"
#include "../MuonTrackScoringTool.h"
#include "../MuonTrackToSegmentTool.h"

using namespace Muon;

DECLARE_COMPONENT(MuonTrackCleaner)
DECLARE_COMPONENT(MuonChamberHoleRecoveryTool)
DECLARE_COMPONENT(MuonSegmentRegionRecoveryTool)
DECLARE_COMPONENT(MuonTrackToSegmentTool)
DECLARE_COMPONENT(MuonSeededSegmentFinder)
DECLARE_COMPONENT(MuonTrackScoringTool)
DECLARE_COMPONENT(MuonTrackExtrapolationTool)
DECLARE_COMPONENT(MuonRefitTool)
DECLARE_COMPONENT(MuonErrorOptimisationTool)
