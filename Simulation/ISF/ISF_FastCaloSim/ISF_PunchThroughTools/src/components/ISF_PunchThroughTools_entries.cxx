#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../PunchThroughTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF,PunchThroughTool)
DECLARE_FACTORY_ENTRIES( ISF_PunchThroughTools ) {
  DECLARE_NAMESPACE_TOOL( ISF,PunchThroughTool)
}

