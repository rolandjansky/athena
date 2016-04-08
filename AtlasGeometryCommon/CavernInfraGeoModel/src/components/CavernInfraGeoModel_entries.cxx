#include "CavernInfraGeoModel/CavernInfraDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(CavernInfraDetectorTool)

DECLARE_FACTORY_ENTRIES(CavernInfraGeoModel) {
    DECLARE_ALGTOOL  ( CavernInfraDetectorTool )
}
