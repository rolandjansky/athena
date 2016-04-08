#include "GaudiKernel/DeclareFactoryEntries.h"

#include "BCM_ZeroSuppression/BCM_ZeroSuppression.h"

DECLARE_ALGORITHM_FACTORY( BCM_ZeroSuppression )

DECLARE_FACTORY_ENTRIES(BCM_ZeroSuppression) {
  DECLARE_ALGORITHM(BCM_ZeroSuppression)
}
