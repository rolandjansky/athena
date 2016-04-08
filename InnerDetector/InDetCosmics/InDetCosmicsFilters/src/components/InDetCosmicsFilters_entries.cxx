#include "InDetCosmicsFilters/TrackFilter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet , TrackFilter )
  
DECLARE_FACTORY_ENTRIES( InDetCosmicsFilters ) 
{
    DECLARE_NAMESPACE_ALGORITHM( InDet ,  TrackFilter )
}
