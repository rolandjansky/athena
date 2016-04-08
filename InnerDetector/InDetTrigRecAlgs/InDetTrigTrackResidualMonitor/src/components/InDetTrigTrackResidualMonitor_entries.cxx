#include "GaudiKernel/DeclareFactoryEntries.h"
#include "InDetTrigTrackResidualMonitor/TrigTrackResidualMonitor.h"
#include "TrkTrack/Track.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( TrigTrackResidualMonitor )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( InDetTrigTrackResidualMonitor )
{
    DECLARE_ALGORITHM( TrigTrackResidualMonitor )
}
