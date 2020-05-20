#include "HGTD_GeoModel/HGTD_DetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(HGTD_DetectorTool)

DECLARE_FACTORY_ENTRIES(HGTD_GeoModel) {
    DECLARE_ALGTOOL(HGTD_DetectorTool)
}
