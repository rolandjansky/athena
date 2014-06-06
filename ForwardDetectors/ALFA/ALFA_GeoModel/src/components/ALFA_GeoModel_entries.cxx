#include "ALFA_GeoModel/ALFA_DetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(ALFA_DetectorTool)

DECLARE_FACTORY_ENTRIES(ALFA_GeoModel) {
    DECLARE_ALGTOOL  ( ALFA_DetectorTool )
}
