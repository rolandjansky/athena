#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../L1TopoTestAlg.h"

DECLARE_ALGORITHM_FACTORY( L1TopoTestAlg )

DECLARE_FACTORY_ENTRIES( L1TopoMonitoring ) {
  DECLARE_ALGORITHM( L1TopoTestAlg )
}
