
#include "../StreamTrigNavSlimming.h"
#include "TrigNavTools/TrigNavigationSlimmingTool.h"
#include "../TrigNavigationSlimming.h"
#include "../TrigNavigationThinningTool.h"
#include "../TrigNavigationThinningSvc.h"


DECLARE_NAMESPACE_TOOL_FACTORY( HLT, TrigNavigationSlimmingTool )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( HLT, TrigNavigationSlimming )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( HLT, StreamTrigNavSlimming )
DECLARE_TOOL_FACTORY( TrigNavigationThinningTool )
DECLARE_SERVICE_FACTORY( TrigNavigationThinningSvc )

