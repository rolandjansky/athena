#include "GaudiKernel/DeclareFactoryEntries.h"

#include "FTK_DataProviderSvc/FTK_DataProviderSvc.h"
#include "FTK_DataProviderSvc/FTK_UncertaintyTool.h"
#include "FTK_DataProviderSvc/FTKFastDataProviderSvc.h"

DECLARE_SERVICE_FACTORY( FTK_DataProviderSvc )
DECLARE_SERVICE_FACTORY( FTKFastDataProviderSvc )
DECLARE_TOOL_FACTORY(FTK_UncertaintyTool)

DECLARE_FACTORY_ENTRIES(FTK_DataProviderSvc) {
  DECLARE_SERVICE( FTK_DataProviderSvc );
  DECLARE_SERVICE( FTKFastDataProviderSvc );
  DECLARE_TOOL(FTK_UncertaintyTool)
}
