#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TileSimUtils/TileSimInfoLoader.h"
#include "TileSimUtils/TileCalibHitNtuple.h"
#include "TileSimUtils/TileCalibHitCntNtup.h"

DECLARE_SERVICE_FACTORY( TileSimInfoLoader )
DECLARE_TOOL_FACTORY( TileCalibHitNtuple )
DECLARE_TOOL_FACTORY( TileCalibHitCntNtup )

DECLARE_FACTORY_ENTRIES(TileSimUtils) {
  DECLARE_SERVICE( TileSimInfoLoader )
  DECLARE_TOOL( TileCalibHitNtuple )
  DECLARE_TOOL( TileCalibHitCntNtup )
}
