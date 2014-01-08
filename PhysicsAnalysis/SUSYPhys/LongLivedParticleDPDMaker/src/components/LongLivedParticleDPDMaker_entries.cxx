#include "GaudiKernel/DeclareFactoryEntries.h"

#include "LongLivedParticleDPDMaker/HIPsFilterAlg.h"
#include "LongLivedParticleDPDMaker/DVFilterAlg.h"
#include "LongLivedParticleDPDMaker/DVFilterAlgEgamma.h"
#include "LongLivedParticleDPDMaker/DVFilterAlgDiElectron.h"
#include "LongLivedParticleDPDMaker/RPVLLTestRates.h"
#include "LongLivedParticleDPDMaker/StealthFilterAlg.h"

DECLARE_ALGORITHM_FACTORY( HIPsFilterAlg )
DECLARE_ALGORITHM_FACTORY( DVFilterAlg )
DECLARE_ALGORITHM_FACTORY( DVFilterAlgEgamma )
DECLARE_ALGORITHM_FACTORY( DVFilterAlgDiElectron )
DECLARE_ALGORITHM_FACTORY( RPVLLTestRates )
DECLARE_ALGORITHM_FACTORY( StealthFilterAlg )

DECLARE_FACTORY_ENTRIES( LongLivedParticleDPDMaker ) {
  DECLARE_ALGORITHM( HIPsFilterAlg  );
  DECLARE_ALGORITHM( DVFilterAlg  );
  DECLARE_ALGORITHM( DVFilterAlgEgamma  );
  DECLARE_ALGORITHM( DVFilterAlgDiElectron  );
  DECLARE_ALGORITHM( RPVLLTestRates  );
  DECLARE_ALGORITHM( StealthFilterAlg  );
}

