#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrackCaloClusterRecAlgs/TrackCaloClusterRecAlg.h"

DECLARE_ALGORITHM_FACTORY(TrackCaloClusterRecAlg)

    
DECLARE_FACTORY_ENTRIES(TrackCaloClusterRecAlgs)
{
  DECLARE_ALGORITHM(TrackCaloClusterRecAlg)      
}