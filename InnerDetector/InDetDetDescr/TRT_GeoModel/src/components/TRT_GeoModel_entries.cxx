#include "TRT_GeoModel/TRT_DetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TRT_DetectorTool)

DECLARE_FACTORY_ENTRIES(TRT_GeoModel) {
    DECLARE_ALGTOOL  ( TRT_DetectorTool )
}
