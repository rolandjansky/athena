
#include "SCT_SLHC_GeoModel/SCT_SLHC_DetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(SCT_SLHC_DetectorTool)

DECLARE_FACTORY_ENTRIES(SCT_SLHC_GeoModel) {
    DECLARE_ALGTOOL  ( SCT_SLHC_DetectorTool )
}
