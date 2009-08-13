#include "GaudiKernel/DeclareFactoryEntries.h"

#include "VP1TrkAuxAlgs/VP1TrkInitializer.h"

DECLARE_ALGORITHM_FACTORY(VP1TrkInitializer)

DECLARE_FACTORY_ENTRIES(VP1TrkAuxAlgs) {
  DECLARE_ALGORITHM( VP1TrkInitializer );
}
