#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TestActionTimerTool.h"
#include "../TestActionVPTimerTool.h"
#include "../TestActionEHistTool.h"

DECLARE_TOOL_FACTORY( G4UA::TestActionTimerTool )
DECLARE_TOOL_FACTORY( G4UA::TestActionVPTimerTool )
DECLARE_TOOL_FACTORY( G4UA::TestActionEHistTool )

DECLARE_FACTORY_ENTRIES( G4ProfilingTools ) {

 DECLARE_TOOL( G4UA::TestActionTimerTool )
 DECLARE_TOOL( G4UA::TestActionVPTimerTool )
 DECLARE_TOOL( G4UA::TestActionEHistTool )

}
