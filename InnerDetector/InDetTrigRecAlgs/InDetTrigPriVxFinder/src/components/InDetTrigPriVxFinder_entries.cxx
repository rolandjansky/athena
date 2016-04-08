#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrigPriVxFinder/TrigVxPrimary.h"
#include "InDetTrigPriVxFinder/TrigVxPrimaryAllTE.h"
#include "TrkTrack/Track.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrigVxPrimary )
DECLARE_ALGORITHM_FACTORY( TrigVxPrimaryAllTE )

DECLARE_FACTORY_ENTRIES( InDetTrigPriVxFinder )
{
    DECLARE_ALGORITHM( TrigVxPrimary )
    DECLARE_ALGORITHM( TrigVxPrimaryAllTE )
}

