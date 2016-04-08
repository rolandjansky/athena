#include "HitAnalysis/CaloHitAnalysis.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( CaloHitAnalysis )

DECLARE_FACTORY_ENTRIES( HitAnalysis ) {
  DECLARE_ALGORITHM( CaloHitAnalysis )
}

