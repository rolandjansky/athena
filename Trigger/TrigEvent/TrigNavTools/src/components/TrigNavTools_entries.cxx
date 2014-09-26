#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../StreamTrigNavSlimming.h"
#include "TrigNavTools/TrigNavigationSlimmingTool.h"
#include "../TrigNavigationSlimming.h"
#include "../TrigNavigationThinningTool.h"


DECLARE_NAMESPACE_TOOL_FACTORY( HLT, TrigNavigationSlimmingTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( HLT, TrigNavigationSlimming )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( HLT, StreamTrigNavSlimming )
DECLARE_TOOL_FACTORY( TrigNavigationThinningTool )

DECLARE_FACTORY_ENTRIES( TrigNavTools ) {
  DECLARE_NAMESPACE_TOOL( HLT, TrigNavigationSlimmingTool );
  DECLARE_NAMESPACE_ALGORITHM( HLT, TrigNavigationSlimming );
  DECLARE_NAMESPACE_ALGORITHM( HLT, StreamTrigNavSlimming );
  DECLARE_TOOL( TrigNavigationSlimmingTool );
}

