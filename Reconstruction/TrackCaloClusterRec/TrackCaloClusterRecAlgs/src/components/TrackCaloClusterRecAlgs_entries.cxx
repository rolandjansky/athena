#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrackCaloClusterRecAlgs/TrackCaloClusterAlg.h"
#include "TrackCaloClusterRecAlgs/TrackCaloClusterInfoAlg.h"

DECLARE_ALGORITHM_FACTORY(TrackCaloClusterAlg)
DECLARE_ALGORITHM_FACTORY(TrackCaloClusterInfoAlg)
DECLARE_ALGORITHM_FACTORY(TrackCaloClusterInfoUFOAlg)

    
DECLARE_FACTORY_ENTRIES(TrackCaloClusterRecAlgs)
{
  DECLARE_ALGORITHM(TrackCaloClusterAlg)
  DECLARE_ALGORITHM(TrackCaloClusterInfoAlg)
  DECLARE_ALGORITHM(TrackCaloClusterInfoUFOAlg)
}
