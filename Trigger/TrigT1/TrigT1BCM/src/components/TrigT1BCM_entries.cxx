#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1BCM/TrigT1BCM.h"

using namespace LVL1;

DECLARE_ALGORITHM_FACTORY(TrigT1BCM)

DECLARE_FACTORY_ENTRIES(TrigT1BCM) {
  DECLARE_ALGORITHM(TrigT1BCM)
}
