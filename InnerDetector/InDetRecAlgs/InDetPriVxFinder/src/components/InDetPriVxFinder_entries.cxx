#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetPriVxFinder/InDetPriVxFinder.h"
#include "InDetPriVxFinder/InDetPriVxDummyFinder.h"
#include "InDetPriVxFinder/InDetVxLinksToTrackParticles.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetPriVxFinder )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetPriVxDummyFinder )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetVxLinksToTrackParticles )

DECLARE_FACTORY_ENTRIES( InDetPriVxFinder )
{
   DECLARE_NAMESPACE_ALGORITHM( InDet, InDetPriVxFinder )
   DECLARE_NAMESPACE_ALGORITHM( InDet, InDetPriVxDummyFinder )
   DECLARE_NAMESPACE_ALGORITHM( InDet, InDetVxLinksToTrackParticles )
}

