#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TileFCSmStepToTileHitVec.h"

DECLARE_ALGORITHM_FACTORY( TileFCSmStepToTileHitVec )

DECLARE_FACTORY_ENTRIES( TileFastCaloSim ) {
  DECLARE_ALGORITHM( TileFCSmStepToTileHitVec )
}
