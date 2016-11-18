#include "../GeoModelSvc.h"
#include "../GeoDbTagSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY(GeoModelSvc)
DECLARE_SERVICE_FACTORY(GeoDbTagSvc)

DECLARE_FACTORY_ENTRIES(GeoModelSvc) {
    DECLARE_SERVICE   ( GeoModelSvc )
    DECLARE_SERVICE   ( GeoDbTagSvc )
}

