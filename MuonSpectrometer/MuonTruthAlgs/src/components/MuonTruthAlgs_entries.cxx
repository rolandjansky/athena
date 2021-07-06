#include "../DetailedMuonPatternTruthBuilder.h"
#include "../MuonDecayTruthTrajectoryBuilder.h"
#include "../MuonSegmentTruthAssociationAlg.h"
#include "../MuonTruthAssociationAlg.h"
#include "../MuonTruthDecorationAlg.h"
#include "MuonTruthAlgs/MuonDetailedTrackTruthMaker.h"
#include "MuonTruthAlgs/MuonPRD_MultiTruthMaker.h"
#include "MuonTruthAlgs/MuonPatternCombinationDetailedTrackTruthMaker.h"
#include "MuonTruthAlgs/MuonTrackTruthTool.h"

using namespace Muon;
using namespace Trk;

DECLARE_COMPONENT(MuonPRD_MultiTruthMaker)
DECLARE_COMPONENT(MuonDetailedTrackTruthMaker)
DECLARE_COMPONENT(MuonPatternCombinationDetailedTrackTruthMaker)
DECLARE_COMPONENT(MuonTruthDecorationAlg)
DECLARE_COMPONENT(MuonTruthAssociationAlg)
DECLARE_COMPONENT(MuonSegmentTruthAssociationAlg)

DECLARE_COMPONENT(MuonTrackTruthTool)
DECLARE_COMPONENT(MuonDecayTruthTrajectoryBuilder)
DECLARE_COMPONENT(DetailedMuonPatternTruthBuilder)
