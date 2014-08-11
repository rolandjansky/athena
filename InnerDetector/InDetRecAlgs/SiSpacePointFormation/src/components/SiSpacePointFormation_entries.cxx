
#include "SiSpacePointFormation/SiTrackerSpacePointFinder.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(InDet, SiTrackerSpacePointFinder)

DECLARE_FACTORY_ENTRIES( SiSpacePointFormation ) 
{
    DECLARE_NAMESPACE_ALGORITHM(InDet, SiTrackerSpacePointFinder )
}

