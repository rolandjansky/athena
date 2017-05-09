 #include "GaudiKernel/DeclareFactoryEntries.h"

#include "../QuirkPhysicsTool.h"
#include "../DebugSteppingActionTool.h"


DECLARE_TOOL_FACTORY( QuirksPhysicsTool )
DECLARE_TOOL_FACTORY( G4UA::DebugSteppingActionTool )


DECLARE_FACTORY_ENTRIES( Quirks ) {
  DECLARE_TOOL( QuirksPhysicsTool )
  DECLARE_TOOL( G4UA::DebugSteppingActionTool )
}
