#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonTruthAlgs/MuonPRD_MultiTruthMaker.h"
#include "MuonTruthAlgs/MuonDetailedTrackTruthMaker.h"
#include "MuonTruthAlgs/MuonPatternCombinationDetailedTrackTruthMaker.h"
#include "MuonTruthAlgs/MuonTrackTruthTool.h"
#include "../MuonDecayTruthTrajectoryBuilder.h"
#include "MuonTruthAlgs/TrackParticleTruthMaker.h"
#include "../DetailedMuonPatternTruthBuilder.h"
#include "../MuonTruthDecorationAlg.h"
#include "../MuonTruthAssociationAlg.h"
#include "../MuonSegmentTruthAssociationAlg.h"
#include "../MuonTruthSummaryAlg.h"


using namespace Muon;
using namespace Trk;

DECLARE_ALGORITHM_FACTORY( MuonPRD_MultiTruthMaker )
DECLARE_ALGORITHM_FACTORY( MuonDetailedTrackTruthMaker )
DECLARE_ALGORITHM_FACTORY( MuonPatternCombinationDetailedTrackTruthMaker )
DECLARE_ALGORITHM_FACTORY( TrackParticleTruthMaker )
DECLARE_ALGORITHM_FACTORY( MuonTruthDecorationAlg )
DECLARE_ALGORITHM_FACTORY( MuonTruthAssociationAlg )
DECLARE_ALGORITHM_FACTORY( MuonSegmentTruthAssociationAlg )
DECLARE_ALGORITHM_FACTORY( MuonTruthSummaryAlg )

DECLARE_TOOL_FACTORY( MuonTrackTruthTool )
DECLARE_TOOL_FACTORY( MuonDecayTruthTrajectoryBuilder )
DECLARE_TOOL_FACTORY( DetailedMuonPatternTruthBuilder )

DECLARE_FACTORY_ENTRIES( MuonTruthAlgs ) 
{
  DECLARE_ALGORITHM( MuonTruthSummaryAlg );
  DECLARE_ALGORITHM( MuonPRD_MultiTruthMaker )
  DECLARE_ALGORITHM( MuonDetailedTrackTruthMaker )
  DECLARE_ALGORITHM( MuonPatternCombinationDetailedTrackTruthMaker )
  DECLARE_ALGORITHM( TrackParticleTruthMaker )
  DECLARE_ALGORITHM( MuonTruthDecorationAlg )
  DECLARE_ALGORITHM( MuonTruthAssociationAlg )
  DECLARE_ALGORITHM( MuonSegmentTruthAssociationAlg )

  DECLARE_TOOL( MuonTrackTruthTool )
  DECLARE_TOOL( MuonDecayTruthTrajectoryBuilder )
  DECLARE_TOOL( DetailedMuonPatternTruthBuilder )
}
