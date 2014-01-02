#include "TrigInDetVxInJetTool/TrigInDetVxInJetTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_TOOL_FACTORY( TrigInDetVxInJetTool )
DECLARE_FACTORY_ENTRIES( TrigInDetVxInJetTool ) {
    DECLARE_TOOL( TrigInDetVxInJetTool );
}
//Notes:
//
//1. The argument to the DECLARE_FACTORY_ENTRIES(XXX) is the name of the
//   component library (libXXX.so).
//2. Each Algorithm that is contained in the library should be declared
//   by the DECLARE_ALGORITHM() statement.
//3. Each Service that is contained in the library should be declared
//   by the DECLARE_SERVICE() statement.
// 
// See Athena User Guide for more information
