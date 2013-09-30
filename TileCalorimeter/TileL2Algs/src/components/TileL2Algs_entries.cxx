#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TileL2Algs/TileRawChannelToL2.h"
#include "TileL2Algs/TileMuRODToNtuple.h"
#include "TileL2Algs/TileL2Builder.h"

DECLARE_ALGORITHM_FACTORY( TileRawChannelToL2 )
DECLARE_ALGORITHM_FACTORY( TileMuRODToNtuple )
DECLARE_TOOL_FACTORY( TileL2Builder )

DECLARE_FACTORY_ENTRIES(TileL2Algs) {
  DECLARE_ALGORITHM( TileRawChannelToL2 )
  DECLARE_ALGORITHM( TileMuRODToNtuple )
  DECLARE_TOOL( TileL2Builder )
}
