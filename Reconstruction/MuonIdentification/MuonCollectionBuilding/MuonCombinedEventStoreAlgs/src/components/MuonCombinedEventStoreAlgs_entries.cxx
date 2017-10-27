#include "../FinalizeMuonContainers.h"
#include "../InitializeMuonClusters.h"
#include "../FinalizeMuonClusters.h"
#include "../InitializeMuonCaloEnergy.h"
#include "../MergeMuonCaloEnergyContainers.h"
#include "../MergeMuonCaloClusterContainers.h"
#include "../MuonToTrackConverterAlg.h"
#include "../MuonTrackSlimmer.h"

DECLARE_COMPONENT( Rec::FinalizeMuonContainers )
DECLARE_COMPONENT( Rec::InitializeMuonClusters )
DECLARE_COMPONENT( Rec::FinalizeMuonClusters )
DECLARE_COMPONENT( Rec::InitializeMuonCaloEnergy )
DECLARE_COMPONENT( Rec::MergeMuonCaloEnergyContainers )
DECLARE_COMPONENT( Rec::MergeMuonCaloClusterContainers )
DECLARE_COMPONENT( MuonToTrackConverterAlg )
DECLARE_COMPONENT( Rec::MuonTrackSlimmer )

