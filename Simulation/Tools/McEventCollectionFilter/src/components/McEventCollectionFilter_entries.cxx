//
//  Entry file for ATHENA component libraries
//

#include "../McEventCollectionFilter.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
 
DECLARE_ALGORITHM_FACTORY( McEventCollectionFilter )

DECLARE_FACTORY_ENTRIES(McEventCollectionFilter) {
  DECLARE_ALGORITHM(McEventCollectionFilter)
}

