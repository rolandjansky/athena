#include "../GeoModelSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY(GeoModelSvc)

DECLARE_FACTORY_ENTRIES(GeoModelSvc) {
    DECLARE_SERVICE   ( GeoModelSvc )
    //    DECLARE_CONVERTER ( GeomodelElemCnv )
}

