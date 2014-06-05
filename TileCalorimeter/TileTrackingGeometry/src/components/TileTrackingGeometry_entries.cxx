#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TileTrackingGeometry/TileVolumeBuilder.h"

using namespace Tile;

DECLARE_TOOL_FACTORY( TileVolumeBuilder )

DECLARE_FACTORY_ENTRIES( TileTrackingGeometry )
{
    DECLARE_TOOL( TileVolumeBuilder );
}


