#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../MuonCompetingClustersOnTrackCreator.h"
#include "../TriggerChamberClusterOnTrackCreator.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonCompetingClustersOnTrackCreator )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, TriggerChamberClusterOnTrackCreator )

DECLARE_FACTORY_ENTRIES( MuonCompetingClustersOnTrackCreator )
{
    DECLARE_NAMESPACE_TOOL( Muon, MuonCompetingClustersOnTrackCreator )
    DECLARE_NAMESPACE_TOOL( Muon, TriggerChamberClusterOnTrackCreator )	
}

