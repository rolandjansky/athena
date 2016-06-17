#include "SCT_GeoModelXml/SCT_GMX_DetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(SCT_GMX_DetectorTool)

DECLARE_FACTORY_ENTRIES(SCT_GeoModelXml) {
    DECLARE_ALGTOOL(SCT_GMX_DetectorTool)
}
