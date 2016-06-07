#include "IOVSvc/IOVSvc.h"
#include "IOVSvc/CondSvc.h"
#include "IOVSvc/IOVSvcTool.h"
#include "../CondInputLoader.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY(IOVSvc)
DECLARE_TOOL_FACTORY( IOVSvcTool )

DECLARE_SERVICE_FACTORY(CondSvc)

DECLARE_ALGORITHM_FACTORY( CondInputLoader )

// DECLARE_FACTORY_ENTRIES(IOVSvc) {
//   DECLARE_SERVICE( IOVSvc );
//   DECLARE_TOOL( IOVSvcTool );
// }

