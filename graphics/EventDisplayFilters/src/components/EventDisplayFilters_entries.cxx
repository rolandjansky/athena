#include "GaudiKernel/DeclareFactoryEntries.h"
#include "EventDisplayFilters/FilterUsingMBTS.h"
#include "EventDisplayFilters/FilterUsingMBTSTiming.h"
#include "EventDisplayFilters/FilterUsingSpacePoints.h"
DECLARE_ALGORITHM_FACTORY( FilterUsingMBTS )
DECLARE_ALGORITHM_FACTORY( FilterUsingMBTSTiming )
DECLARE_ALGORITHM_FACTORY( FilterUsingSpacePoints )
DECLARE_FACTORY_ENTRIES( EventDisplayFilters ) {
  DECLARE_ALGORITHM( FilterUsingMBTSTiming )
  DECLARE_ALGORITHM( FilterUsingSpacePoints )
}
