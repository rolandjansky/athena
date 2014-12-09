#include "AGDD2GeoSvc/AGDD2GeoSvc.h"
#include "AGDD2GeoSvc/DefaultAGDDTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY(AGDDtoGeoSvc)
DECLARE_TOOL_FACTORY(DefaultAGDDTool)

DECLARE_FACTORY_ENTRIES(AGDDtoGeoSvc) {
    DECLARE_SERVICE   ( AGDDtoGeoSvc )
	DECLARE_ALGTOOL	  ( DefaultAGDDTool )
}

