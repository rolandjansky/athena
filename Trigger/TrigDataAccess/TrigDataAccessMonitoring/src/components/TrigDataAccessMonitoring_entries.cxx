#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigDataAccessMonitoring/MonROBDataProviderSvc.h"

DECLARE_SERVICE_FACTORY( MonROBDataProviderSvc )

DECLARE_FACTORY_ENTRIES( TrigDataAccessMonitoring ) {
  DECLARE_SERVICE( MonROBDataProviderSvc );
}
