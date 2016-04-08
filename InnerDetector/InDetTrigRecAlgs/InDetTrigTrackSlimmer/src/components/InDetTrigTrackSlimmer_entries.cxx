#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrigTrackSlimmer/TrigTrackSlimmer.h"
#include "TrkTrack/Track.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrigTrackSlimmer )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( InDetTrigTrackSlimmer )
{
    DECLARE_ALGORITHM( TrigTrackSlimmer )
}
