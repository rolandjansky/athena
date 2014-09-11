#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/SegMemSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY( ActiveStoreSvc )
DECLARE_SERVICE_FACTORY( StoreGateSvc )
DECLARE_SERVICE_FACTORY( SegMemSvc )

DECLARE_FACTORY_ENTRIES(StoreGate) {
    DECLARE_SERVICE( ActiveStoreSvc )
    DECLARE_SERVICE( StoreGateSvc )
    DECLARE_SERVICE( SegMemSvc )
}
