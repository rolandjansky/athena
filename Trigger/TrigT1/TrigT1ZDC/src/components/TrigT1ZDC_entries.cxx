#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigT1ZDC/TrigT1ZDC.h"

using namespace LVL1;

DECLARE_ALGORITHM_FACTORY(TrigT1ZDC)

DECLARE_FACTORY_ENTRIES(TrigT1ZDC) {
  DECLARE_ALGORITHM(TrigT1ZDC)
}
