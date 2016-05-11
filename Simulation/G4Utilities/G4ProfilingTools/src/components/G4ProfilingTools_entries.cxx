#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TestActionEHist.h"
#include "../TestActionTimer.h"
#include "../TestActionTimerTool.h"
#include "../TestActionVPTimerTool.h"
#include "../TestActionEHistTool.h"
#include "../TestActionVPTimer.h"

DECLARE_TOOL_FACTORY( TestActionEHist )
DECLARE_TOOL_FACTORY( TestActionTimer )
DECLARE_TOOL_FACTORY( G4UA::TestActionTimerTool )
DECLARE_TOOL_FACTORY( G4UA::TestActionVPTimerTool )
DECLARE_TOOL_FACTORY( G4UA::TestActionEHistTool )
DECLARE_TOOL_FACTORY( TestActionVPTimer )

DECLARE_FACTORY_ENTRIES( G4ProfilingTools ) {

 DECLARE_TOOL( TestActionEHist )
 DECLARE_TOOL( TestActionTimer )
 DECLARE_TOOL( G4UA::TestActionTimerTool )
 DECLARE_TOOL( G4UA::TestActionVPTimerTool )
 DECLARE_TOOL( G4UA::TestActionEHistTool )
 DECLARE_TOOL( TestActionVPTimer )

}
