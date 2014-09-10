#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1MBTS/TrigT1MBTS.h"

using namespace LVL1;

DECLARE_ALGORITHM_FACTORY(TrigT1MBTS)

DECLARE_FACTORY_ENTRIES(TrigT1MBTS) {
  DECLARE_ALGORITHM(TrigT1MBTS)
}
