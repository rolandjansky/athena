#include "IOVSvc/IOVSvc.h"
#include "IOVSvc/IOVSvcTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY(IOVSvc)
DECLARE_TOOL_FACTORY( IOVSvcTool )

DECLARE_FACTORY_ENTRIES(IOVSvc) {
  DECLARE_SERVICE( IOVSvc );
  DECLARE_TOOL( IOVSvcTool );
}

