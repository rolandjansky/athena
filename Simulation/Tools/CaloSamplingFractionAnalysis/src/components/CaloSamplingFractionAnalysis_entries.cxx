#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../LarEMSamplingFraction.h"

DECLARE_ALGORITHM_FACTORY( LarEMSamplingFraction )

DECLARE_FACTORY_ENTRIES( CaloSamplingFractionAnalysis ) {
  DECLARE_ALGORITHM( LarEMSamplingFraction )
}

