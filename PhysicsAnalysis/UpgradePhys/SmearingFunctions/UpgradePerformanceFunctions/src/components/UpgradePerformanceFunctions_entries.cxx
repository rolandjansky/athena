// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"
#include "UpgradePerformanceFunctions/UpgradePerformanceFunctionsAlg.h"

DECLARE_NAMESPACE_TOOL_FACTORY(Upgrade, UpgradePerformanceFunctions)
DECLARE_ALGORITHM_FACTORY(UpgradePerformanceFunctionsAlg)

DECLARE_FACTORY_ENTRIES(UpgradePerformanceFunctions) {
  DECLARE_NAMESPACE_TOOL(Upgrade, UpgradePerformanceFunctions)
  DECLARE_ALGORITHM(UpgradePerformanceFunctionsAlg)
}
