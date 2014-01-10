#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "../sTgcFastDigitizer.h"
#include "../MM_FastDigitizer.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

DECLARE_ALGORITHM_FACTORY( sTgcFastDigitizer )
DECLARE_ALGORITHM_FACTORY( MM_FastDigitizer )

DECLARE_FACTORY_ENTRIES(MuonFastDigitization) {
  DECLARE_ALGORITHM( sTgcFastDigitizer )
  DECLARE_ALGORITHM( MM_FastDigitizer )
}
