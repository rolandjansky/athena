#include "TrigT2CaloTileMon/T2CaloTileMon.h"
#include "TrigT2CaloTileMon/TileMonHadEnFex.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( T2CaloTileMon )
DECLARE_TOOL_FACTORY( TileMonHadEnFex )

DECLARE_FACTORY_ENTRIES(TrigT2CaloTileMon) {
    DECLARE_ALGORITHM( T2CaloTileMon );
    DECLARE_TOOL( TileMonHadEnFex );
}
