#include "GaudiKernel/DeclareFactoryEntries.h"

#include "FTK_DataProviderSvc/FTK_DataProviderSvc.h"

DECLARE_SERVICE_FACTORY( FTK_DataProviderSvc )

DECLARE_FACTORY_ENTRIES(FTK_DataProviderSvc) {
  DECLARE_SERVICE( FTK_DataProviderSvc );
}
