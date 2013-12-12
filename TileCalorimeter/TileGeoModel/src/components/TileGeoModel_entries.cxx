#include "TileGeoModel/TileDetectorTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TileDetectorTool)

DECLARE_FACTORY_ENTRIES(TileGeoModel) {
    DECLARE_ALGTOOL  ( TileDetectorTool )
}
