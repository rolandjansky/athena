//
//  Entry file for ATHENA component libraries
//

#include "../McEventCollectionFilter.h"
#include "../TruthResetAlg.h"
#include "../TruthClosureCheck.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
 
DECLARE_ALGORITHM_FACTORY( McEventCollectionFilter )
DECLARE_ALGORITHM_FACTORY( TruthResetAlg )
DECLARE_ALGORITHM_FACTORY( TruthClosureCheck )

DECLARE_FACTORY_ENTRIES(McEventCollectionFilter) {
  DECLARE_ALGORITHM(McEventCollectionFilter)
}

