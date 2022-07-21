#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../PunchThroughTool.h"
#include "../PunchThroughClassifier.h"

DECLARE_NAMESPACE_TOOL_FACTORY( ISF,PunchThroughTool)
DECLARE_NAMESPACE_TOOL_FACTORY( ISF,PunchThroughClassifier)

DECLARE_FACTORY_ENTRIES( ISF_PunchThroughTools ) {
  DECLARE_NAMESPACE_TOOL( ISF,PunchThroughTool)
  DECLARE_NAMESPACE_TOOL( ISF,PunchThroughClassifier)
}
