#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../ValidationTests/AtlasCLHEP_RandomGenerators_test.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( AtlasCLHEP_RandomGenerators , AtlasCLHEP_RandomGenerators_test )

DECLARE_FACTORY_ENTRIES(AtlasCLHEP_RandomGenerators) {
    DECLARE_NAMESPACE_ALGORITHM( AtlasCLHEP_RandomGenerators , AtlasCLHEP_RandomGenerators_test );
}
