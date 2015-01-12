#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../IsolationBuilder.h"

DECLARE_ALGORITHM_FACTORY( IsolationBuilder )

DECLARE_FACTORY_ENTRIES(IsolationAlgs) {
  DECLARE_ALGORITHM( IsolationBuilder )
}
