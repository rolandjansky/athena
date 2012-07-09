
#include "SCT_GeoModel/SCT_DetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(SCT_DetectorTool)

DECLARE_FACTORY_ENTRIES(SCT_GeoModel) {
    DECLARE_ALGTOOL  ( SCT_DetectorTool )
}
