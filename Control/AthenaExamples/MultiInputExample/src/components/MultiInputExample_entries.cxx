#include "MultiInputExample/MyMultiInputAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( MyMultiInputAlg )

DECLARE_FACTORY_ENTRIES(MultiInputExample) {
DECLARE_ALGORITHM( MyMultiInputAlg );
}
