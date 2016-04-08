// AsgExampleTools_entries.cxx

#include "../AsgExampleAlgorithm.h"
#include "AsgExampleTools/AsgHelloTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(AsgHelloTool)

DECLARE_ALGORITHM_FACTORY(AsgExampleAlgorithm)

DECLARE_FACTORY_ENTRIES(AsgExampleTools) {
  DECLARE_TOOL(AsgHelloTool)
  DECLARE_ALGORITHM(AsgExampleAlgorithm)
}

