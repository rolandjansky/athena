#include "TrkDetDescrSvc/TrackingGeometrySvc.h"
#include "TrkDetDescrSvc/TrackingVolumesSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace Trk;

DECLARE_SERVICE_FACTORY( TrackingGeometrySvc )
DECLARE_SERVICE_FACTORY( TrackingVolumesSvc )

DECLARE_FACTORY_ENTRIES(TrkDetDescrSvc) {
    DECLARE_SERVICE   ( TrackingGeometrySvc )
    DECLARE_SERVICE   ( TrackingVolumesSvc )
}
