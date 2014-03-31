#include "InDetLowBetaFinder/LowBetaAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace InDet;

DECLARE_ALGORITHM_FACTORY( LowBetaAlg )
  
DECLARE_FACTORY_ENTRIES(InDetLowBetaFinder) {
  DECLARE_ALGORITHM( LowBetaAlg )
    }
