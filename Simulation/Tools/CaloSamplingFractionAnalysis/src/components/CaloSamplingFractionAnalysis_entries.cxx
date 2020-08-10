#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../LarEMSamplingFraction.h"
#include "../LArFCalSamplingFraction.h"

DECLARE_ALGORITHM_FACTORY( LarEMSamplingFraction )
DECLARE_ALGORITHM_FACTORY( LArFCalSamplingFraction )

DECLARE_FACTORY_ENTRIES( CaloSamplingFractionAnalysis ) {
  DECLARE_ALGORITHM( LarEMSamplingFraction )
  DECLARE_ALGORITHM( LArFCalSamplingFraction )
}

