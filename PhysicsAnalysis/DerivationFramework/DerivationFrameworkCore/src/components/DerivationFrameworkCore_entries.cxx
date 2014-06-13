#include "DerivationFrameworkCore/DerivationKernel.h"
#include "DerivationFrameworkCore/CommonAugmentation.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( DerivationFramework , DerivationKernel )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( DerivationFramework , CommonAugmentation )


DECLARE_FACTORY_ENTRIES(DerivationFrameworkCore) {
 DECLARE_NAMESPACE_ALGORITHM( DerivationFramework , DerivationKernel )
 DECLARE_NAMESPACE_ALGORITHM( DerivationFramework , CommonAugmentation )
}
