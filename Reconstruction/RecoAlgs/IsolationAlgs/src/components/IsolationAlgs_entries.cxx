#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../IsolationBuilder.h"
#include "../IsolationPflowCorrection.h"

DECLARE_ALGORITHM_FACTORY( IsolationBuilder )
DECLARE_ALGORITHM_FACTORY( IsolationPflowCorrection )

DECLARE_FACTORY_ENTRIES(IsolationAlgs) {
  DECLARE_ALGORITHM( IsolationBuilder )
  DECLARE_ALGORITHM( IsolationPflowCorrection )
}
