#include "InDetServMatGeoModel/InDetServMatTool.h"
#include "InDetServMatGeoModel/InDetServMatBuilderToolSLHC.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_TOOL_FACTORY(InDetServMatTool)
DECLARE_TOOL_FACTORY(InDetServMatBuilderToolSLHC)

DECLARE_FACTORY_ENTRIES(InDetServMatGeoModel) {
    DECLARE_ALGTOOL  ( InDetServMatTool )
    DECLARE_ALGTOOL  ( InDetServMatBuilderToolSLHC )
}
