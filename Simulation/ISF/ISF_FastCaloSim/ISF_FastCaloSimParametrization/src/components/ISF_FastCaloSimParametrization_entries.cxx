#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_FastCaloSimParametrization/FastCaloSimParamAlg.h"
#include "ISF_FastCaloSimParametrization/ISF_HitAnalysis.h"
#include "ISF_FastCaloSimParametrization/NativeFastCaloSimSvc.h"

DECLARE_ALGORITHM_FACTORY( FastCaloSimParamAlg )
DECLARE_ALGORITHM_FACTORY( ISF_HitAnalysis) 
DECLARE_NAMESPACE_SERVICE_FACTORY( ISF , NativeFastCaloSimSvc )

DECLARE_FACTORY_ENTRIES(FastCaloSimParamAlg) {
  DECLARE_ALGORITHM( FastCaloSimParamAlg )
  DECLARE_ALGORITHM( ISF_HitAnalysis )
}

DECLARE_FACTORY_ENTRIES( ISF_FastCaloSimParametrization ) {
  DECLARE_NAMESPACE_SERVICE( ISF , NativeFastCaloSimSvc )
}
