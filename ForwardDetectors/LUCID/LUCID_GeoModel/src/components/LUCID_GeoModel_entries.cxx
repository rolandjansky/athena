#include "LUCID_GeoModel/LUCID_DetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(LUCID_DetectorTool)

DECLARE_FACTORY_ENTRIES(LUCID_GeoModel) {
    DECLARE_ALGTOOL  ( LUCID_DetectorTool )
}
