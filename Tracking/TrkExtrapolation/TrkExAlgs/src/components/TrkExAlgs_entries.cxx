#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkExAlgs/ExtrapolatorTest.h"
#include "TrkExAlgs/CombinedExtrapolatorTest.h"
#include "TrkExAlgs/CETmaterial.h"
#include "TrkExAlgs/ExtrapolationValidation.h"
#include "TrkExAlgs/EnergyLossExtrapolationValidation.h"
#include "TrkExAlgs/RiddersAlgorithm.h"

using namespace Trk;

DECLARE_ALGORITHM_FACTORY( RiddersAlgorithm )
DECLARE_ALGORITHM_FACTORY( ExtrapolatorTest )
DECLARE_ALGORITHM_FACTORY( ExtrapolationValidation )
DECLARE_ALGORITHM_FACTORY( EnergyLossExtrapolationValidation )
DECLARE_ALGORITHM_FACTORY( CombinedExtrapolatorTest )
DECLARE_ALGORITHM_FACTORY( CETmaterial )

DECLARE_FACTORY_ENTRIES( TrkExAlgs ) {
  DECLARE_ALGORITHM( RiddersAlgorithm )
  DECLARE_ALGORITHM( ExtrapolatorTest )
  DECLARE_ALGORITHM( ExtrapolationValidation )
  DECLARE_ALGORITHM( EnergyLossExtrapolationValidation )
  DECLARE_ALGORITHM( CombinedExtrapolatorTest )
  DECLARE_ALGORITHM( CETmaterial )
}   
