#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TileCosmicAlgs/TileMuonFitter.h"
#include "TileCosmicAlgs/TileCosmicsTrigger.h"

DECLARE_ALGORITHM_FACTORY( TileMuonFitter )
DECLARE_ALGORITHM_FACTORY( TileCosmicsTrigger )

DECLARE_FACTORY_ENTRIES(TileCosmicAlgs) {
  DECLARE_ALGORITHM( TileMuonFitter )
  DECLARE_ALGORITHM( TileCosmicsTrigger )
}
