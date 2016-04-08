#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../TestActionEHist.h"
#include "../TestActionTimer.h"
#include "../TestActionVPTimer.h"

DECLARE_TOOL_FACTORY( TestActionEHist )
DECLARE_TOOL_FACTORY( TestActionTimer )
DECLARE_TOOL_FACTORY( TestActionVPTimer )

DECLARE_FACTORY_ENTRIES( G4ProfilingTools ) {

 DECLARE_TOOL( TestActionEHist )
 DECLARE_TOOL( TestActionTimer )
 DECLARE_TOOL( TestActionVPTimer )

}
