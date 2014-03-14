#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FastCaloSimParametrization/FastCaloSimParamAlg.h"
#include "ISF_FastCaloSimParametrization/ISF_HitAnalysis.h"

DECLARE_ALGORITHM_FACTORY( FastCaloSimParamAlg )
DECLARE_ALGORITHM_FACTORY( ISF_HitAnalysis) 

DECLARE_FACTORY_ENTRIES(FastCaloSimParamAlg) {
  DECLARE_ALGORITHM( FastCaloSimParamAlg )
  DECLARE_ALGORITHM( ISF_HitAnalysis )
}
