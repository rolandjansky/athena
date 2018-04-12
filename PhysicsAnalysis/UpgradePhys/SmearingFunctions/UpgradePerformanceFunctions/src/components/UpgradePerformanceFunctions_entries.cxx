// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "UpgradePerformanceFunctions/UpgradePerformanceFunctions.h"

DECLARE_NAMESPACE_TOOL_FACTORY(Upgrade, UpgradePerformanceFunctions)

DECLARE_FACTORY_ENTRIES(UpgradePerformanceFunctions) {
  DECLARE_NAMESPACE_TOOL(Upgrade, UpgradePerformanceFunctions)
}
