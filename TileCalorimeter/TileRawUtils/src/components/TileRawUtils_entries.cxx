#include "TileRawUtils/TileRoI_Map.h" 
#include "TileRawUtils/TileRegionSelectorTable.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( TileRoI_Map )
DECLARE_TOOL_FACTORY( TileRegionSelectorTable )

DECLARE_FACTORY_ENTRIES(TileRawUtils) {
  DECLARE_TOOL( TileRegionSelectorTable )
}
