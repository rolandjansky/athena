 #include "GaudiKernel/DeclareFactoryEntries.h"

#include "../QuirkPhysicsTool.h"
#include "../DebugSteppingAction.h"


DECLARE_TOOL_FACTORY( QuirksPhysicsTool )
DECLARE_TOOL_FACTORY( DebugSteppingAction )


DECLARE_FACTORY_ENTRIES( Quirks ) {
  DECLARE_TOOL( QuirksPhysicsTool )
  DECLARE_TOOL( DebugSteppingAction )
}
