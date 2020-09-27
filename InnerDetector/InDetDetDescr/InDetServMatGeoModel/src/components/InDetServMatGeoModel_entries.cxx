#include "InDetServMatGeoModel/InDetServMatTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_TOOL_FACTORY(InDetServMatTool)

DECLARE_FACTORY_ENTRIES(InDetServMatGeoModel) {
    DECLARE_ALGTOOL  ( InDetServMatTool )
}
