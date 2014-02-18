#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1Lucid/TrigT1Lucid.h"

using namespace LVL1;

DECLARE_ALGORITHM_FACTORY(TrigT1Lucid)

DECLARE_FACTORY_ENTRIES(TrigT1Lucid) {
  DECLARE_ALGORITHM(TrigT1Lucid)
}
