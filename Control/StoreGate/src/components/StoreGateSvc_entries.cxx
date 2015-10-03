#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/tools/SGImplSvc.h"
#include "StoreGate/SegMemSvc.h"
#include "../src/SGHiveMgrSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_SERVICE_FACTORY( ActiveStoreSvc )
DECLARE_SERVICE_FACTORY( StoreGateSvc )
DECLARE_SERVICE_FACTORY( SGImplSvc )
DECLARE_SERVICE_FACTORY( SegMemSvc )

#ifdef ATHENAHIVE
DECLARE_NAMESPACE_SERVICE_FACTORY( SG, HiveMgrSvc )
#endif

DECLARE_FACTORY_ENTRIES(StoreGate) {
    DECLARE_SERVICE( ActiveStoreSvc )
    DECLARE_SERVICE( SGImplSvc )
    DECLARE_SERVICE( StoreGateSvc )
    DECLARE_SERVICE( SegMemSvc )
    DECLARE_NAMESPACE_SERVICE( SG, HiveMgrSvc )
}
