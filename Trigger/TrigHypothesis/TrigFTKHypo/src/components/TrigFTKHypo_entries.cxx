#include "GaudiKernel/DeclareFactoryEntries.h"
#include "src/TrigFTKAvailableHypo.h"

DECLARE_ALGORITHM_FACTORY(TrigFTKAvailableHypo)

DECLARE_FACTORY_ENTRIES(TrigFTKHypo) {
  DECLARE_ALGORITHM(TrigFTKAvailableHypo)
}
