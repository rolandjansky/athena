#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../FinalizeMuonContainers.h"
#include "../InitializeMuonClusters.h"
#include "../FinalizeMuonClusters.h"
#include "../InitializeMuonCaloEnergy.h"
#include "../MergeMuonCaloEnergyContainers.h"
#include "../MergeMuonCaloClusterContainers.h"
#include "../MuonToTrackConverterAlg.h"
#include "../MuonTrackSlimmer.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Rec, FinalizeMuonContainers )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( Rec, InitializeMuonClusters )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( Rec, FinalizeMuonClusters )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( Rec, InitializeMuonCaloEnergy )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( Rec, MergeMuonCaloEnergyContainers )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( Rec, MergeMuonCaloClusterContainers )
DECLARE_ALGORITHM_FACTORY ( MuonToTrackConverterAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY ( Rec, MuonTrackSlimmer )

DECLARE_FACTORY_ENTRIES( MuonCombinedEventStoreAlgs )
{
  DECLARE_NAMESPACE_ALGORITHM( Rec, FinalizeMuonContainers )
  DECLARE_NAMESPACE_ALGORITHM ( Rec, InitializeMuonClusters )
  DECLARE_NAMESPACE_ALGORITHM ( Rec, FinalizeMuonClusters )
  DECLARE_NAMESPACE_ALGORITHM ( Rec, InitializeMuonCaloEnergy )
  DECLARE_NAMESPACE_ALGORITHM ( Rec, MergeMuonCaloEnergyContainers )
  DECLARE_NAMESPACE_ALGORITHM ( Rec, MergeMuonCaloClusterContainers )
  DECLARE_ALGORITHM (MuonToTrackConverterAlg )
  DECLARE_NAMESPACE_ALGORITHM ( Rec, MuonTrackSlimmer )
}
