#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TauTools/TauToolAthenaExample.h"
#include "TauTools/TauCommonDetailsMerger.h"
#include "TauTools/FindFactors.h"

DECLARE_ALGORITHM_FACTORY( TauToolAthenaExample )
DECLARE_ALGORITHM_FACTORY( TauCommonDetailsMerger )
DECLARE_ALGORITHM_FACTORY( FindFactors )

DECLARE_FACTORY_ENTRIES( TauTools ) {
  DECLARE_ALGORITHM( TauToolAthenaExample )
  DECLARE_ALGORITHM( TauCommonDetailsMerger )
  DECLARE_ALGORITHM( FindFactors )
}
