#include "GaudiKernel/DeclareFactoryEntries.h"
#include "VP1AlgsBatch/VP1BatchOnLatestEvent.h"


DECLARE_ALGORITHM_FACTORY(VP1BatchOnLatestEvent)

DECLARE_FACTORY_ENTRIES(VP1AlgsBatch) {
  DECLARE_ALGORITHM( VP1BatchOnLatestEvent );
}
