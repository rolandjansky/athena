/**
 * @file AthenaPoolTools_entries.cxx
 * @brief Required by Gaudi class factory
 */
  
#include "../EventCount.h"
#include "../MetadataTest.h"
#include "../RequireUniqueEvent.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(EventCount)
DECLARE_ALGORITHM_FACTORY(MetadataTest)
DECLARE_ALGORITHM_FACTORY(RequireUniqueEvent)

DECLARE_FACTORY_ENTRIES(AthenaPoolTools) {
   DECLARE_ALGORITHM(EventCount)
   DECLARE_ALGORITHM(MetadataTest)
   DECLARE_ALGORITHM(RequireUniqueEvent)
}
