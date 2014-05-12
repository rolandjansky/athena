#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrigROBDataProviderSvc.h"
#include "../TrigROBDataProviderSvc_RTT.h"

DECLARE_SERVICE_FACTORY( TrigROBDataProviderSvc )
DECLARE_SERVICE_FACTORY( TrigROBDataProviderSvc_RTT )

DECLARE_FACTORY_ENTRIES( TrigROBDataProviderSvc ) {
  DECLARE_SERVICE( TrigROBDataProviderSvc );
  DECLARE_SERVICE( TrigROBDataProviderSvc_RTT );
}


