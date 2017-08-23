#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TileGeoG4SDTool.h"
#include "../TileGeoG4SDCalc.hh"

DECLARE_TOOL_FACTORY( TileGeoG4SDTool )
DECLARE_SERVICE_FACTORY( TileGeoG4SDCalc )

DECLARE_FACTORY_ENTRIES( TileGeoG4SD ) {
  DECLARE_TOOL( TileGeoG4SDTool )
  DECLARE_SERVICE( TileGeoG4SDCalc )
}
