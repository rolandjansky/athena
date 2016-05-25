#include "LArMonitoring/LArHVMapTool.h"
#include "LArMonitoring/LArTesting.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY(LArTesting)
DECLARE_TOOL_FACTORY(LArHVMapTool)

DECLARE_FACTORY_ENTRIES(LArMonitoring) {
  DECLARE_ALGORITHM(LArTesting)
  DECLARE_ALGTOOL(LArHVMapTool)
}
